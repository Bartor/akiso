#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/limits.h>

int waitc = 1;

void signal_handler(int no) {
	if (no == 17) {
		waitpid(-1, NULL, WNOHANG);
	} else if (no == 2) {
		//ignore
	}
}

int promptRead (char* outputarray[]) {
	char input[PATH_MAX];
	char* word;
	char s[2] = " ";
	
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	printf("[\e[38;5;160mLSH \e[38;5;33m%s\e[0m] ", cwd);
	
	fgets(input, PATH_MAX, stdin);
	if (strlen(input) > 0 && input[strlen(input) - 1] == '\n') {
		input[strlen(input) - 1] = '\0';
	}

	int i = 0;
	
	word = strtok(input, s);
	while (word != NULL) {
		outputarray[i] = calloc(strlen(word) + 1, sizeof(char));
		strcpy(outputarray[i], word);
		i++;
		word = strtok(NULL, s);
	}
	return i;
}

void pipeThrough(char* pipedCommands[], int size) {

	int foundIndex = size;
	int commandsCount = 0;
	char* commands[size][size];
	for (int i = size - 1; i > -1; i--) {
		
		if (pipedCommands[i][0] == '|' || i == 0) {
			
			int k = 0;
			int j = i + 1;
			if (i == 0) {
				j = 0;
			}
			for (; j < foundIndex; j++) {
				commands[commandsCount][k] = calloc(strlen(pipedCommands[j]) + 1, sizeof(char));
				strcpy(commands[commandsCount][k], pipedCommands[j]);
				k++;
			}
			commands[commandsCount][k] = NULL;
			
			commandsCount++;
			foundIndex = i;
		}
	}
	
	int fd[commandsCount - 1][2];
	for (int i = 0; i < commandsCount - 1; i++) {
		if (pipe(fd[i]) == -1) {
			perror("pipe error");
		}
		//printf("creating fd[%d] 0:%d 1:%d\n", i, fd[i][0], fd[i][1]);
	}
	
	int pids[commandsCount];
	int status, wpid;
	//printf("command count: %d\n\n", commandsCount);
	
	for (int i = 0; i < commandsCount; i++) {
		//printf("iteration over i=%d childPid=%d command=%s:\n", i, childPid, commands[i][0]);
		if ((pids[i] = fork()) == 0) {

			if (commandsCount > 1) {
				if (i == 0) {
					//printf("connecting child%d %s 0 to %d\n", i, commands[i][0], fd[i][0]);
					if (dup2(fd[i][0], 0) == -1) {
						perror("dup2 error on i=0");
					}
				} else if (i != commandsCount - 1) {
					//printf("connecting child%d %s 1 to %d 0 to %d\n", i, commands[i][0], fd[i-1][1], fd[i][0]);
					if (dup2(fd[i-1][1], 1) == -1) {
						perror("dup2 error");
					}
					if (dup2(fd[i][0], 0) == -1) {
						perror("dup2 error");
					}
				} else {
					//printf("connecting child%d %s 1 to %d\n", i, commands[i][0], fd[i-1][1]);
					if (dup2(fd[i-1][1], 1) == -1) {
						perror("dup2 error on i=commands-1");
					}
				}
			}
			
			for (int j = 0; j < commandsCount - 1; j++) {
				close(fd[j][0]);
				close(fd[j][1]);
			}
	
			execvp(commands[i][0], commands[i]);
			perror("execvperror");
			_exit(1);
			
		}
	}
	
	for (int j = 0; j < commandsCount - 1; j++) {
		close(fd[j][0]);
		close(fd[j][1]);
	}
	
	if(waitc) {
		for(int i = commandsCount - 1; i > -1; i--) {
			if(pids[i] > 0) {
				int status;
				//printf("waiting for %d\n", pids[i]);
				waitpid(pids[i], &status, 0);
				//printf("process %d exited with %d\n", pids[i], status);
			} else {
				//printf("why the fuck pid on %d is %d\n", i, pids[i]);
			}
		}
	}

	
	exit(0);
}

int main() {
	char* words[PATH_MAX];
	
	signal(SIGCHLD, signal_handler);
	//signal(SIGINT, signal_handler);
	
	while(1) {
		int i = promptRead(words);

		if (i == 0) {
			 continue;
		}
		
		int status, wpid;
		
		if (words[0][0] == 'c' && words[0][1] == 'd' && words[0][2] == '\0') {
			chdir(words[1]);
			continue;
		} else if (words[0][0] == 'e' && words[0][1] == 'x' && words[0][2] == 'i' && words[0][3] == 't' && words[0][4] == '\0') {
			kill(0, 9);
			exit(0);
		}
		
		if (words[i-1][0] == '&' && words[i-1][1] == '\0') {
			waitc = 0;
			i--;
		} else {
			waitc = 1;
		}
		
		int child = fork();
		
		if (child == 0) {
			pipeThrough(words, i);
		} else {
			if (waitc) {
				wpid = waitpid(child, &status, 0);
			}
		}
	}
	return 0;
}
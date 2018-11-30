#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <linux/limits.h>

// global variable indicating if we should wait for command(s) to finish
int waitc = 1;

// handles our funny signals
void signal_handler(int no) {
	
	// if signal is SIGCHLD, let's wait for it
	if (no == 17) {
		waitpid(-1, NULL, WNOHANG);
	
	// we want to ignore SIGINT
	} else if (no == 2) {}
}

// printf our prompt, reads from console to given array of strings, return number of words
int promptRead (char* outputarray[]) {
	char input[PATH_MAX];
	char* word;
	
	// will be used by strtok as separator
	char s[2] = " ";
	
	// currect directory
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	
	// print
	printf("[\e[38;5;160mLSH \e[38;5;33m%s\e[0m] ", cwd);
	
	// read
	fgets(input, PATH_MAX, stdin);
	// ignore newline chars
	if (strlen(input) > 0 && input[strlen(input) - 1] == '\n') {
		input[strlen(input) - 1] = '\0';
	}
	
	// will hold our return value
	int i = 0;
	
	// let's split it
	word = strtok(input, s);
	while (word != NULL) {
		outputarray[i] = calloc(strlen(word) + 1, sizeof(char));
		strcpy(outputarray[i], word);
		i++;
		word = strtok(NULL, s);
	}
	return i;
}

// sets fds to files we specify
// modes:
// 0 - read
// 1 - write
// -1 - none
// 2 - all
void addRedirects(int size, char* commands[][size], int index, int mode) {
	
	int commandLength = 0;
	while (commands[index][commandLength] != NULL) commandLength++;
	
	for(int j = 0; j < commandLength; j++) {
		
		if(commands[index][j][0] == '>' && (mode == 1 || mode == 2)) {
			
			char path[PATH_MAX];
			for(int k = 1; k < strlen(commands[index][j]); k++) {
				path[k-1] = commands[index][j][k];
			}
			
			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("> error");
				_exit(1);
			}
			
			if (dup2(file, 1) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}
						
			commands[index][j] = NULL;
							
		} else if (commands[index][j][0] == '2' && commands[index][j][1] == '>') {
					
			char path[PATH_MAX];
			for(int k = 2; k < strlen(commands[index][j]); k++) {
				path[k-2] = commands[index][j][k];
			}
						
			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("2> error");
				_exit(1);
			}
			if (dup2(file, 2) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}
					
			commands[index][j] = NULL;
			
		} else if (commands[index][j][0] == '<' && (mode == 0 || mode == 2)) {
			
			char path[PATH_MAX];
			for(int k = 1; k < strlen(commands[index][j]); k++) {
				path[k-1] = commands[index][j][k];
			}
			
			int file;
			if ((file = open(path, O_RDWR | O_CREAT | O_APPEND)) == -1) {
				perror("> error");
				_exit(1);
			}
			
			if (dup2(file, 0) == -1) {
				perror("dup2 error when tryging to redirect to file");
			}
						
			commands[index][j] = NULL;

		}
	}
}

// it's called pipe although it handles all command execs
void pipeThrough(char* pipedCommands[], int size) {

	// stores index for last | symbol
	int foundIndex = size;
	int commandsCount = 0;
	char* commands[size][size];
	for (int i = size - 1; i > -1; i--) {
		
		if (pipedCommands[i][0] == '|' || i == 0) {
			
			int k = 0;
			
			// we want to ignore | unless we're on the first char, then there's no | to ignore
			int j = i + 1;
			if (i == 0) {
				j = 0;
			}
			// split each command into words
			for (; j < foundIndex; j++) {
				// commands is allocated and written to and yet we don't ever free it, because the process dies eventually, so, whatever
				commands[commandsCount][k] = calloc(strlen(pipedCommands[j]) + 1, sizeof(char));
				strcpy(commands[commandsCount][k], pipedCommands[j]);
				k++;
			}
			commands[commandsCount][k] = NULL;
			
			commandsCount++;
			foundIndex = i;
		}
	}
	
	// create fds shared between forks later, we need one less than processes we want to connect
	int fd[commandsCount - 1][2];
	for (int i = 0; i < commandsCount - 1; i++) {
		if (pipe(fd[i]) == -1) {
			perror("pipe error");
		}
	}
	
	// we will store processes pids to wait for them later
	int pids[commandsCount];
	int status, wpid;

	for (int i = 0; i < commandsCount; i++) {
		if ((pids[i] = fork()) == 0) {
			
			if (commandsCount > 1) {
				if (i == 0) {
					// case for the LAST command in pipes
					if (dup2(fd[i][0], 0) == -1) {
						perror("dup2 error on i=0");
					}
					
					addRedirects(size, commands, i, 1);
					
				} else if (i != commandsCount - 1) {
					// case for all the MIDDLE commands in pipes
					if (dup2(fd[i-1][1], 1) == -1) {
						perror("dup2 error");
					}
					if (dup2(fd[i][0], 0) == -1) {
						perror("dup2 error");
					}
					
					addRedirects(size, commands, i, -1);
					
				} else {
					// case for the FRIST command in pipes
					if (dup2(fd[i-1][1], 1) == -1) {
						perror("dup2 error on i=commands-1");
					}
					
					addRedirects(size, commands, i, 0);
				}
			} else {
				// if we have only one command, we add all redirects to it
				addRedirects(size, commands, 0, 2);
			}
			
			// we close all unused fds - important!
			for (int j = 0; j < commandsCount - 1; j++) {
				close(fd[j][0]);
				close(fd[j][1]);
			}
			
			// we execute the command - process is replaced by it and dies off eventually, freeing resources
			execvp(commands[i][0], commands[i]);
			// if we get to this part, execvp failed
			perror("execvp error");
			_exit(1);
			
		}
	}
	
	// close fds in parten
	for (int j = 0; j < commandsCount - 1; j++) {
		close(fd[j][0]);
		close(fd[j][1]);
	}
	
	// if we decided to wait, wait for all
	if(waitc) {
		for(int i = commandsCount - 1; i > -1; i--) {
			if(pids[i] > 0) {
				int status;
				waitpid(pids[i], &status, 0);
			} else {
				// if pid isn't greater than 0 then... what
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
		
		// cd and exit are handled in main process
		if (words[0][0] == 'c' && words[0][1] == 'd' && words[0][2] == '\0') {
			chdir(words[1]);
			continue;
		} else if (words[0][0] == 'e' && words[0][1] == 'x' && words[0][2] == 'i' && words[0][3] == 't' && words[0][4] == '\0') {
			kill(0, 9);
			exit(0);
		}
		
		// waitc is handled in main process
		if (words[i-1][0] == '&' && words[i-1][1] == '\0') {
			waitc = 0;
			i--;
		} else {
			waitc = 1;
		}
		
		// let's fork and leave all the commands handling to fork
		int child = fork();
		
		if (child == 0) {
			pipeThrough(words, i);
		} else {
			if (waitc) {
				wpid = waitpid(child, &status, 0);
			}
		}
		
		// let's free memory before allocating it again
		while (0 < --i) free(words[i]);
	}
	return 0;
}
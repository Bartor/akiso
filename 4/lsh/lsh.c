#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/limits.h>

void signal_handler(int no) {
	if (no = 17) {
		waitpid(-1, NULL, WNOHANG);
	}
}

int main() {
	char input[PATH_MAX];
	char * word;
	char * words[1024]; //na razie nie obsługujemy powyżej 1024 słów w komendzie
	char s[2] = " ";
	
	signal(SIGCHLD, signal_handler);
	
	while(1) {
		int waitc = 1;
		
		char cwd[PATH_MAX];
		getcwd(cwd, sizeof(cwd));
		printf("[\e[38;5;160mLSH \e[38;5;33m%s\e[0m] ", cwd);
		gets(input);
		
		word = strtok(input, s);
		
		int i = 0;
		while (word != NULL) {
			words[i] = malloc(strlen(word) + 1);
			strcpy(words[i], word);
			i++;
			word = strtok(NULL, s);
		}
		if (words[i - 1][0] == '&') {
			waitc = 0;
			words[i - 1] = NULL;
		} else {
			words[i] = NULL;
		}
		
		int status, wpid;
		
		if (words[0][0] == 'c' && words[0][1] == 'd' && words[0][2] == '\0') {
			chdir(words[1]);
			continue;
		} else if (words[0][0] == 'e' && words[0][1] == 'x' && words[0][2] == 'i' && words[0][3] == 't' && words[0][4] == '\0') {
			while ((wpid = wait(&status)) > 0);
			break;
		}
		
		int child = fork();
		
		if (child == 0) {
			execvp(words[0], words);
			exit(0);
		} else {
			if (waitc) {
				wpid = waitpid(child, &status, 0);
			}
			//printf("Parent process: %d\n", w);
		}
	}
	return 0;
}
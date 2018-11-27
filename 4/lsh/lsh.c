#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/limits.h>

/* Handler sygnałów
 * przjmuje sygnały 17 i czeka na proces, aby go zabić 
 * oraz 2 i ignoruje go
 */
void signal_handler(int no) {
	if (no == 17) {
		waitpid(-1, NULL, WNOHANG);
	} else if (no == 2) {
		//ignore
	}
}

/* Funkcja wywołyana do wyświetlenia podstawowego promptu
 * wpisytwania oraz utworzenia tablicy stringów po rozdzieleniu
 * go znakiem zawartym w s. Zwraca długość gotowej tablicy.
 */
int promptRead (char* outputarray[]) {
	char input[PATH_MAX];
	char* word;
	char s[2] = " ";
	
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	printf("[\e[38;5;160mLSH \e[38;5;33m%s\e[0m] ", cwd);
	
	gets(input);
	
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

int main() {
	char* words[PATH_MAX];
	
	signal(SIGCHLD, signal_handler);
	//signal(SIGINT, signal_handler);
	
	while(1) {
		int waitc = 1;
		
		int i = promptRead(words);
		
		if (i == 0) {
			 continue;
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
			kill(0, 9);
			exit(0);
		}
		
		int child = fork();
		
		if (child == 0) {
			execvp(words[0], words);
			exit(0);
		} else {
			if (waitc) {
				wpid = waitpid(child, &status, 0);
			}
		}
	}
	return 0;
}
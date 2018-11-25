#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

int signal_handled = 0;

void sig_handler(int signo) {
	printf("received %d\n", signo);
	signal_handled = 1;
}

int main() {
	pid_t pid;
	pid = fork();
	if(pid == 0) {
		if(signal(SIGUSR1, sig_handler) == SIG_ERR) printf("cannot catch SIGUSR1\n");
		if(signal(SIGINT, sig_handler) == SIG_ERR) printf("cannot catch SIGUSR1\n");
		printf("child process here! I'm receiving the signals\n");
		while(1) {
			if (signal_handled == 1) {
				printf("signal handled, stopping child process\n");
				exit(0);
			}
			printf("waiting for signals...\n");
			sleep(1);
		}
	} else {
		int status;
		sleep(2);
		kill(pid, SIGUSR1);
		kill(pid, SIGINT);
		kill(pid, SIGUSR1);
		kill(pid, SIGINT);
		(void) waitpid(pid, &status, 0);
	}
	return 0;
}
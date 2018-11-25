#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signo) {
	printf("received\n");
}

int main() {
	if(signal(SIGINT, sig_handler) == SIG_ERR) printf("cannot catch SIGINT\n");
	if(signal(SIGKILL, sig_handler) == SIG_ERR) printf("cannot catch SIGKILL\n");
	if(signal(SIGSTOP, sig_handler) == SIG_ERR) printf("cannot catch SIGSTOP\n");
	if(signal(SIGUSR1, sig_handler) == SIG_ERR) printf("cannot catch SIGUSR1\n");
	while(1) {
		printf("working\n");
		sleep(1);
	}
	return 0;
}
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signo) {
	printf("received %d\n", signo);
}

int main() {
	for (int i = 1; i <= 64; i++) {
		if (signal(i, sig_handler) == SIG_ERR) printf("cannot catch %d\n", i);
	}
	while(1) {
		printf("working\n");
		sleep(1);
	}
	return 0;
}
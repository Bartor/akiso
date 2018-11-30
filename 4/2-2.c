#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
	setuid(0);
	printf("%d\n", kill(1, SIGKILL)); //nie da się tego zrobić, bo man 2 kill
	return 0;
}
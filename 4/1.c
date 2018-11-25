#include <stdlib.h>

int main() {
	char* name[2];
	name[0] = "bash";
	name[1] = NULL;
	setuid(0);
	execvp("/bin/bash", name);
}
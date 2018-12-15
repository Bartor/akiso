#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <linux/limits.h>

//compile with gcc -m32 -fno-stack-protector 1.c [-o 1] -lm

//converts given number to number of given base up to 16 and puts it into out
void convert(char* out, int number, int base) {
	int i = 63;
	int j = 0;
	if (number < 0) {
		number = 0 - number;
		j = 1;
		out[0] = '-';
	}
	do {
		out[i] = "0123456789ABCDEF"[number % base];
		i--;
		number = number/base;
	} while (number > 0);
	while (++i < 64) {
		out[j++] = out[i];
	}
	out[j] = '\0';
}

//converts the number stored in string to an integer
int deconvert(char* string, int base) {
	char numbers[17] = "0123456789ABCDEF";
	int result = 0;
	int negative = 0;
	if (string[0] == '-') negative = 1;
	int numLen = strlen(string);
	for (int i = numLen - 1; i > negative - 1; i--) {
		for (int j = 0; j < base + 1; j++) {
			if (string[i] == numbers[j]) {
				result += j*pow(base, numLen - 1 - i);
			}
		}
	}
	if (negative) return result*-1;
	else return result;
}


void myprintf(char* pattern, ...) {
	char *p = (char *) &pattern + sizeof(pattern);
	
	for(int i = 0; i < strlen(pattern); i++) {
		if (pattern[i] == '%' && i < strlen(pattern) - 1) {
			char output[1024];
			switch (pattern[i+1]) {
				case 'd': {
					int* number;
					number = ((int *)p);
					p += sizeof(int);
					convert(output, *number, 10);
					write(1, output, strlen(output));
					i+=2;;
					break;
				}
				case 's': {
					char* text;
					text = *((char **) p);
					p += sizeof(char*);
					while (text[0] != '\0') {
						write(1, text, sizeof(char));
						text++;
					}
					i+=2;
					break;
				}
				case 'x': {
					int number;
					number = *((int *)p);
					p += sizeof(int);
					convert(output, number, 16);
					write(1, output, strlen(output));
					i+=2;
					break;
				}
				case 'b': {
					int number;
					number = *((int *)p);
					p += sizeof(int);
					convert(output, number, 2);
					write(1, output, strlen(output));
					i+=2;
					break;
				}
				default: {
					write(1, pattern[i], 1);
					break;
				}
			}
		}
		int a = write(1, &pattern[i], 1);
	}
	p = NULL;
	write(1, "\n\0", 3);
}

int myscanf(const char* pattern, ...) {
	char* p = (char *) &pattern + sizeof pattern;
	
	char* input = calloc(1024, sizeof(char));
	int size = read(0, input, 1024);
	if (input[size - 1] == '\n') input[size-1] = '\0';
	input[size] = '\0';
	
	if (!strcmp(pattern, "%d")) {
		int* in;
		in = (int*)(*(int*)p);
		p += sizeof(int*);
		*in = deconvert(input, 10);
	} else if (!strcmp(pattern, "%s")) {
		char** in;
		in = (char **)(*(char**) p);
		*in = input;
		p += sizeof(*in);
	} else if (!strcmp(pattern, "%x")) {
		int* in;
		in = (int*)(*(int*)p);
		p += sizeof(int*);
		*in = deconvert(input, 16);
	} else if (!strcmp(pattern, "%b")) {
		int* in;
		in = (int*)(*(int*)p);
		p += sizeof(int*);
		*in = deconvert(input, 2);
	}
	p = NULL;
	return size;
}

int main(void) {
	char* a;
	myprintf("Enter a string ");
	myscanf("%s", &a);
	int b;
	myprintf("Enter an int ");
	myscanf("%d", &b);
	int c;
	myprintf("Enter a bin ");
	myscanf("%b", &c);
	int d;
	myprintf("Enter a hex ");
	myscanf("%x", &d);

	myprintf("You entered:\nstring: %s\nformat:\tdec\tbin\thex\t\nint:\t%d\t%b\t%x\nbin:\t%d\t%b\t%x\nhex:\t%d\t%b\t%x", a, b, b, b, c, c, c, d, d, d);
	return 0;
}
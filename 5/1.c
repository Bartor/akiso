#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <linux/limits.h>

void convert(char* out, int number, int base) {
	int i = 63;
	int j = 0;
	if (number < 0) {
		number = 0 - number;
		out[--i] = '-';
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

int deconvert(char* string, int base) {
	char numbers[17] = "0123456789ABCDEF";
	int result = 0;
	int negative = 0;
	if (string[0] == '-') negative = 1;
	int numLen = strlen(string);
	for (int i = numLen - 1; i > negative - 1; i--) {
		for (int j = 0; j < 17; j++) {
			if (string[i] == numbers[j]) {
				result += j*pow(base, numLen - i - 1);
				break;
			}
		}
	}
	if (negative) return -1*result;
	else return result;
}


void myprintf(char* pattern, ...) {
	va_list vl;
	va_start(vl, pattern);

	for(int i = 0; i < strlen(pattern); i++) {
		if (pattern[i] == '%' && i < strlen(pattern) - 1) {
			char output[1024];
			switch (pattern[i+1]) {
				case 'd': {
					int number;
					number = va_arg(vl, int);
					convert(output, number, 10);
					write(1, output, strlen(output));
					i+=2;;
					break;
				}
				case 's': {
					char* text;
					text = va_arg(vl, char*);
					write(1, text, strlen(text));
					i+=2;
					break;
				}
				case 'x': {
					int number;
					number = va_arg(vl, int);
					convert(output, number, 16);
					write(1, output, strlen(output));
					i+=2;
					break;
				}
				case 'b': {
					int number;
					number = va_arg(vl, int);
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
	write(1, "\n\0", 3);
}

int main(void) {
	myprintf("PIESEK %s TO %b FAJNE %d ZWIERZE %x WIELCE", "?", 123, 123, 123);
	printf("%d\n", deconvert("-AB", 16));
	return 0;
}
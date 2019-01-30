// 1a - alternative
// it doesn't really work that well, but it gives funny results, so i kept it

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * e.g.
 * 0.3 = fractionToBinary(3, 10, n)
 * 0.03 = fractionToBinary(3, 100, n)
 * etc.
 */
 
char* fractionToBinaryString(int number, int position, int size) {
	char* out = calloc(size, sizeof(char));
	
	for (int i = 0; i < size; i++) {
		number *= 2;
		out[i] = number >= position ? '1' : '0';
		number %= position;
	}
	
	return out;
}


/*
 * adds two strings of 1s and 0s of the same size
 */

char* binaryStringAddition(char* a, char* b, int size) {
	char* out = calloc(size, sizeof(char));
	int carry = 0;
	
	while (size-->0) {
		if (a[size] == '0' && b[size] == '0') {
			if (carry) {
				out[size] = '1';
				carry = 0;
			} else {
				out[size] = '0';
			}
		} else if ((a[size] == '1' && b[size] == '0') || (a[size] == '0' && b[size] == '1')) {
			if (carry) {
				out[size] = '0';
			} else {
				out[size] = '1';
			}
		} else {
			if (carry) {
				out[size] = '1';
			} else {
				out[size] = '0';
				carry = 1;
			}
		}
	}
	
	return out;
}

int main(void) {
	int precision = 500;
	
	char* input = calloc(1000, sizeof(char));
	
	printf("0.");
	scanf("%s", input);
	
	int size = strlen(input);

	char* current = calloc(precision, sizeof(char));
	char* previous = calloc(precision, sizeof(char));
	
	previous = fractionToBinaryString(input[0] - '0', 10, precision);
	
	for (int i = 1; i < size; i++) {
		current = fractionToBinaryString(input[i] - '0', pow(10, i+1), precision);
		previous = binaryStringAddition(previous, current, precision);
	}
	
	printf("0.%s\n", previous);
	
	return 0;
}
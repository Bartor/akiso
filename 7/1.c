#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* multiply(char* string, int n) { //assumes that string is a decimal number, eg. 02323901 or so
	int carry = 0;
	int len = strlen(string);
	
	char* out = calloc(len + 2, sizeof(char)); //we add 1 for the possible overflow and 1 for terminating \0
	
	out[len+1] = '\0';
	
	while(len-->0) {
		int c = string[len] - '0';
		out[len+1] = (c*n + carry) % 10 + '0';
		carry = (c*n + carry)/10;
	}
	
	out[0] = carry + '0';
	
	return out;
}

void shift_left(char* string) {
	int len = strlen(string);
	
	for(int i = 0; i < len - 1; i++) {
		string[i] = string[i+1];
	}
	string[len-1] = '\0';
}

void parse(char* in, char* integer, char* fraction) {
	int len = strlen(in);
	
	int local = 0;
	int dot = 0;
	for (int i = 0; i < len; i++) {
		if (in[i] == '.') {
			integer[local + 1] = '\0';
			local = 0;
			dot = 1;
			continue;
		}
		
		if (!dot) {
			integer[local] = in[i];
		} else {
			fraction[local] = in[i];
		}
		local++;
	}
	
	integer[len] = '\0';
	fraction[local] = '\0';
}

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: [number] [precision]\n");
		return 1;
	}
	
	int precision = atoi(argv[2]);
	
	char* integer = calloc(strlen(argv[1]) + 1, sizeof(char));
	char* fraction = calloc(strlen(argv[1]) + 2, sizeof(char));
	
	parse(argv[1], integer, fraction);
	
	char* out = calloc(precision + 1, sizeof(char));

	out[precision] = '\0';
	
	for (int i = 0; i < precision; i++) {
		fraction = multiply(fraction, 2);
		if (fraction[0] == '1') {
			out[i] = '1';
			shift_left(fraction);
			fraction[1] == '0';
		} else {
			shift_left(fraction);
			out[i] = '0';
		}
	}
	
	printf("0.%s\n", out);
	
	return 0;
}
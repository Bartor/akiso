#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char first[17];
	char second[17];
} pair;

pair opcodes[9] = {
	{"Load", "0001"},
	{"Store", "0010"},
	{"Add", "0011"},
	{"Subt", "0100"},
	{"Input", "0101"},
	{"Output", "0110"},
	{"Halt", "0111"},
	{"Skipcond", "1000"},
	{"Jump", "1001"}
};

int search_opcode(char* opcode) {
	for (int i = 0; i < 9; i++) {
		if (strcmp(opcode, opcodes[i].first) == 0) {
			return i;
		}
	}
	return -1;
}

int verify(char* code) {
	for (int i = 0; i < strlen(code); i++) {
		if (code[i] != '0' && code[i] != '1') return 0;
	}
	return 1;
}

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

int main(void) {
	int LINE_SIZE = 40;
	int MAX_LINES = 100;
	
	//delimiter
	char d[2] = " ";
	
	pair* labels = calloc(1, sizeof(pair));
	int labelCount = 0;
	
	char** lines = calloc(MAX_LINES, sizeof(char*));
	
	//program counter
	int pc = 0;
	
	size_t len = 0;
	ssize_t read;
	
	char* line = NULL;
	
	printf("\n---LOADING CODE AND SEARCHING FOR LABELS---\n\nLabels found:\n");
	while((read = getline(&line, &len, stdin)) != -1) {
		if (line[read-1] == '\n') line[read-1] = '\0';
		
		lines[pc] = calloc(LINE_SIZE, sizeof(char));
		
		char* word = strtok(line, d);
		
		if (search_opcode(word) == -1) {
			strcpy(labels[labelCount].first, word);
			
			labels = (pair*) realloc(labels, sizeof(pair)*(labelCount+2));
			
			char* bin = calloc(17, sizeof(char));
			convert(bin, pc*16, 2);
			
			strcpy(labels[labelCount].second, bin);
			
			printf("%-16s = %s\n", labels[labelCount].first, labels[labelCount].second);
			
			labelCount++;
			
			strncpy(lines[pc], line+strlen(word)+1, 17);
		} else {
			line[strlen(word)] = ' ';
			strcpy(lines[pc], line);
		}
		
		pc++;
	}
	
	printf("\n---DONE, INTERPRETING---\n\n");
	
	char* output = calloc(MAX_LINES*16 + 1, sizeof(char));
	strcpy(output, "");
	
	for (int i = 0; i < pc; i++) {
		char* line = calloc(LINE_SIZE, sizeof(char));
		strcpy(line, lines[i]);
		
		char padded[17];
		
		char* word = strtok(line, d);
		
		int opcode = search_opcode(word);
		
		if (opcode == -1) {
			
			if (!verify(word)) {
				printf("ERROR: %s is not a valid value\n", word);
				return 1;
			}
			
			sprintf(padded, "%.*s%s", strlen(word) >= 16 ? 0 : (int)(16-strlen(word)), "0000000000000000", word);
			strcat(output, padded);
		} else {
			strcat(output, opcodes[opcode].second);
			
			//get the second operand
			word = strtok(NULL, d);

			int flag = 0;
			for (int j = 0; j < labelCount; j++) {
				//printf("Comparing %s (%d) with %s (%d)\n", word, strlen(word), labels[j].first, strlen(labels[j].first));
				if (strcmp(word, labels[j].first) == 0) {
					//printf("bingo!\n");
					
					sprintf(padded, "%.*s%s", strlen(labels[j].second) >= 12 ? 0 : (int)(12-strlen(labels[j].second)), "000000000000", labels[j].second);
					
					//printf("%s\n", padded);
					strcat(output, padded);
					
					//printf("%s\n", output);
					flag = 1;
					break;
				}
			}
			
			if (flag) continue;
			
			if (!verify(word)) {
				printf("ERROR: %s is not a valid adress\n", word);
				return 1;
			}
			sprintf(padded, "%.*s%s", strlen(word) >= 12 ? 0 : (int)(12-strlen(word)), "000000000000", word);
			strcat(output, padded);
		}
	}
	
	printf("\n---DONE, PRINTING BYTECODE---\n\n");
	
	printf("%s\n", output);
	
	return 0;
}
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h> 

int size;
int **A, **B, **R; //A[row][col]
int rowsDone = 0;

void* f(void* argv) {
	for (int r = rowsDone; r < size; rowsDone++) {
		for(int i = 0; i < size; i++) {
			for(int j = 0; j < size; j++) {
				printf("XD\n");
				if (A[r][i] & B[i][j]) R[r][j] = 0;
				else {
					R[r][j] = 1;
					break;
				}
			}
		}
	}
}

void boolm(int*** arr, int size) {
	srand(time(NULL));
	*arr = (int**) calloc(size, sizeof(int*));
	for (int i = 0; i < size; i++) {
		(*arr)[i] = (int *) calloc(size, sizeof(int));
		for (int j = 0; j < size; j++) (*arr)[i][j] = rand()&0x1;
	}
}


int main(int argc, char** argv) {
	if (argc < 3) {
		printf("usage: ./2 [size of the matrix] [number of threads]\n");
		return -1;
	}
	
	size = strtol(argv[1], NULL, 10);
	int tnumber = strtol(argv[2], NULL, 10);
	
	boolm(&A, size);
	boolm(&B, size);
	boolm(&R, size);
	
	pthread_t tids[tnumber];
	
	for (int i = 0; i < tnumber; i++) {
		pthread_create(tids[i], NULL, f, NULL);
		rowsDone++;
	}
	return 0;
}
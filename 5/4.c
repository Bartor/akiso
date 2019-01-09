#include <stdio.h>
#include <stdlib.h>


void m(int*** arr, int size) {
	*arr = (int**) calloc(size, sizeof(int*));
	for (int i = 0; i < size; i++) {
		(*arr)[i] = (int *) calloc(size, sizeof(int));
	}
}

void v(int*** arr, int size, int* val) {
	srand(time(NULL));
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
			if (val == NULL) (*arr)[i][j] = rand();
			else (*arr)[i][j] = *val;
}

int main(int argc, char** argv) {
	if (argc < 3) {
		printf("usage: ./2 [size of the matrix] [0 - use normal, 1 - use trans]\n");
		return -1;
	}
	
	int size = strtol(argv[1], NULL, 10);
	int t = strtol(argv[2], NULL, 2);
	
	int **A, **B, **tB, **R;
	int rval = 0;
	m(&A, size);
	v(&A, size, NULL);
	m(&B, size);
	v(&B, size, NULL);
	
	m(&tB, size);
	
	m(&R, size);
	v(&R, size, &rval);
	
	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++) tB[i][j] = B[j][i];
	
	printf("Staring... ");
	if (t) printf("using cache optimization\n");
	else printf("no cache optimization\n");
	for(int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				if (t) R[i][j] += A[i][k] * tB[j][k];
				else R[i][j] += A[i][k] * B[k][j];
			}
		}
	}
	printf("Done!\n");
	return 0;
}
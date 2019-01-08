#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

typedef struct {
	int size;
	int** m1;
	int** m2;
	int** res;
} matrices;

//if fill > 0, fills the matrix with rand() % fill integers
//if fill = 0, leaves the matrix empty
int** alloc_matrix(int size, int fill) {
	srand(time(NULL));
	
	int** matrix = calloc(size, sizeof(int*));
	
	for (int i = 0; i < size; i++) {
		matrix[i] = calloc(size, sizeof(int));
		if (fill > 0) {
			for (int j = 0; j < size; j++) matrix[i][j] = rand() % fill;
		}
	}
	
	return matrix;
}

//mutex will lock on any changes of the row
pthread_mutex_t mutex;
int curr_row = 0;

void* mult(void* vargp) {
	matrices* m = (matrices*) vargp;
	
	while (curr_row < m->size) {

		pthread_mutex_lock(&mutex);
		int row = curr_row;
		curr_row++;
		pthread_mutex_unlock(&mutex);
		
		for (int i = 0; i < m->size; i++) {
			for (int j = 0; j < m->size; j++) {
				m->res[row][i] |= (m->m1[row][j] & m->m2[j][i]);
				if (m->res[row][i] == 1) break;
			}
		}
	}
	
	pthread_exit((void*) 0);
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("usage: [size of the matrices] [number of threads]\n");
		return 1;
	}
	
	
	int size = atoi(argv[1]);
	int tnum = atoi(argv[2]);
	
	matrices m;
	
	m.size = size;
	
	m.m1 = alloc_matrix(size, 2);
	m.m2 = alloc_matrix(size, 2);
	m.res = alloc_matrix(size, 0);
	
	pthread_mutex_init(&mutex, NULL);
	pthread_t threadIds[tnum];
	
	for (int i = 0; i < tnum; i++) {
		printf("thread %d starting\n", i);
		pthread_create(&threadIds[i], NULL, &mult, &m);
	}
	
	for (int i = 0; i < tnum; i++) {
		pthread_join(threadIds[i], NULL);
		printf("thread %d finished\n", i);
	}
	
	pthread_mutex_destroy(&mutex);
	
	printf("done, exiting\n");
	return 0;
}
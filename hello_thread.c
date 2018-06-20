#include <stdio.h>
#include <pthread.h>
#define NUM_THREAD 4

void *hello (void *arg) {
	printf("Hello Thread\n");
	return 0;
}

int main (void) {
	int i;

	pthread_t tid[NUM_THREAD];
	for (i = 0; i < NUM_THREAD; i++) { 
		pthread_create(&tid[i], NULL, hello, NULL);
	}

	for (i = 0; i < NUM_THREAD; i++ ) {
		pthread_join(tid[i], NULL);
	}

	return 0;
}
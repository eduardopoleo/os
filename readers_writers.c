#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_READERS 5
#define NUMBER_OF_WRITERS 5

void *read(void *threadArgs) {
	pthread_exit(0);
}

void *write(void *threadArgs) {
	pthread_exit(0);
}

int main(int argc, char **argv) {
	pthread_t readers[NUMBER_OF_READERS];
	pthread_t writers[NUMBER_OF_WRITERS];

	int readerNum[NUMBER_OF_READERS];
	int writerNum[NUMBER_OF_WRITERS];

	for(int i = 0; i < NUMBER_OF_READERS; i++) {
		// memory address of the thread
		// properties of the thread
		// thread function
		// address of the input to the read function 
		// (address requires to be somewhere)
		readerNum[i] = i;
		pthread_create(&readers[i], NULL, read, &readerNum[i]);
	}

	for(int i = 0; i < NUMBER_OF_WRITERS; i++) {
		// memory address of the thread
		// properties of the thread
		// thread function
		// address of the input to the read function 
		// (address requires to be somewhere) I guess it's easier to
		// disasemble.

		writerNum[i] = i;
		pthread_create(&writers[i], NULL, write, &writerNum[i]);
	}

	for(int i = 0; i < NUMBER_OF_READERS; i++) {
		// TODO: why does join need a second argument?
		pthread_join(readers[i], NULL);
	}

	for(int i = 0; i < NUMBER_OF_WRITERS; i++) {
		// TODO: why does join need a second argument?
		pthread_join(readers[i], NULL);
	}

	return 0;
}
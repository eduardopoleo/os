#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_READERS 5
#define NUMBER_OF_WRITERS 5
	
pthread_mutex_t gShareMemoryLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gReadPhase = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWritePhase = PTHREAD_COND_INITIALIZER;

int gwaitingReaders = 0;
int gReadersNum = 0;
int gshareValue = 0;

void *read(void *threadArgs) {
	int id = *((int*)threadArgs);

	pthread_mutex_lock(&gShareMemoryLock);
		gwaitingReaders++;
		while(gReadersNum == -1) {
			pthread_cond_wait(&gReadPhase, &gShareMemoryLock);
		}
		gwaitingReaders--;
		gReadersNum++;
	pthread_mutex_unlock(&gShareMemoryLock);

	printf("READING: id: %d, share value: %d, gReadersNum: %d\n", id, gshareValue, gReadersNum);

	pthread_mutex_lock(&gShareMemoryLock);
		gReadersNum--;
		// can't these guys also wake up the readers. Does it make sense?
		// This seems to prioritize the writer over the readers
		if(gReadersNum == 0)	{
			pthread_cond_signal(&gWritePhase);
		}
	pthread_mutex_unlock(&gShareMemoryLock);

	pthread_exit(0);
}

void *write(void *threadArgs) {
	int id = *(int*)threadArgs;

	pthread_mutex_lock(&gShareMemoryLock);
		while(gReadersNum != 0) {
			pthread_cond_wait(&gWritePhase, &gShareMemoryLock);
		}
		gReadersNum = -1;
	pthread_mutex_unlock(&gShareMemoryLock);

	gshareValue++;

	printf("WRITING: id: %d, share value %d, gReadersNum %d\n:", id, gshareValue, gReadersNum);

	pthread_mutex_lock(&gShareMemoryLock);
		gReadersNum = 0;
		if(gwaitingReaders > 0) {
			pthread_cond_broadcast(&gReadPhase);
		} else {
			pthread_cond_signal(&gWritePhase);
		}
	pthread_mutex_unlock(&gShareMemoryLock);

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
		pthread_join(readers[i], NULL);
	}

	return 0;
}
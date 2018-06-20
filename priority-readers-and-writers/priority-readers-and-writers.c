/* ============================================================================
 * Introduction to Operating Systems
 * CS 8803, GT OMSCS
 *
 * Unauthorized copying of this file, via any medium is strictly prohibited.
 * 
 * "priority-readers-and-writers.c"
 * Implements the "Priority Readers and Writers" in Problem Set 1.
============================================================================ */
#include "priority-readers-and-writers.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* CONSTANTS =============================================================== */
#define NUM_READERS 5
#define NUM_READS 5
#define NUM_WRITERS 5
#define NUM_WRITES 5

/* GLOBAL SHARED DATA ====================================================== */
unsigned int gSharedValue = 0;
pthread_mutex_t gSharedMemoryLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gReadPhase = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWritePhase = PTHREAD_COND_INITIALIZER;
int gWaitingReaders = 0, gReaders = 0;

int main(int argc, char **argv) {

	int i;

	int readerNum[NUM_READERS];
	int writerNum[NUM_WRITERS];

	pthread_t readerThreadIDs[NUM_READERS];
	pthread_t writerThreadIDs[NUM_WRITERS];
	
	// Seed the random number generator
	srandom((unsigned int)time(NULL));

	// Start the readers
	for(i = 0; i < NUM_READERS; i++) {
		readerNum[i] = i;
		pthread_create(&readerThreadIDs[i], NULL, readerMain, &readerNum[i]);
	}

	// Start the writers
	for(i = 0; i < NUM_WRITERS; i++) {
		writerNum[i] = i;
		pthread_create(&writerThreadIDs[i], NULL, writerMain, &writerNum[i]);
	}	

	// Wait on readers to finish
	for(i = 0; i < NUM_READERS; i++) {
		pthread_join(readerThreadIDs[i], NULL);
	}

	// Wait on writers to finish
	for(i = 0; i < NUM_WRITERS; i++) {
		pthread_join(writerThreadIDs[i], NULL);
	}

  return 0;		
}

void *readerMain(void *threadArgument) {
	// cast the threadArgument pointer 
	int id = *((int*)threadArgument);
	int i = 0, numReaders = 0;	

	for(i = 0; i < NUM_READS; i++) {
		// Wait so that reads and writes do not all happen at once
	  usleep(1000 * (random() % NUM_READERS + NUM_WRITERS));

		// Enter critical section
		// this whold set up it's so that we can ensure that we're always gonna have
		// as many readers reading but only 1 writer and nothing else.
	  pthread_mutex_lock(&gSharedMemoryLock);
	  	gWaitingReaders++;
	  	while (gReaders == -1) {
	  		// The mutex is used to protect the condition variable itself.
	  		// This is why wait also is needs the mutex. Cuz wait, mutex, conditions var
	  		// are very intertwieen concepts. You want your threads to wake up
	  		// but you do not want it more than 1 to to through the critical path 
	  		// even if they have reached it before.
	  		pthread_cond_wait(&gReadPhase, &gSharedMemoryLock);
	  	}
	  	gWaitingReaders--;
	  	numReaders = ++gReaders;	  	
	  pthread_mutex_unlock(&gSharedMemoryLock);

	  // Read data
	  fprintf(stdout, "[r%d] reading %u  [readers: %2d]\n", id, gSharedValue, numReaders);

	  // Exit critical section
	  pthread_mutex_lock(&gSharedMemoryLock);
	  	gReaders--;
	  	if (gReaders == 0) {
	  		pthread_cond_signal(&gWritePhase);
	  	}	  	
	  pthread_mutex_unlock(&gSharedMemoryLock);
	}

	pthread_exit(0);
}

void *writerMain(void *threadArgument) {

	int id = *((int*)threadArgument);
	int i = 0, numReaders = 0;	

	for(i = 0; i < NUM_WRITES; i++) {
	  // Wait so that reads and writes do not all happen at once
	  usleep(1000 * (random() % NUM_READERS + NUM_WRITERS));

		// Enter critical section
	  pthread_mutex_lock(&gSharedMemoryLock);
	  	while (gReaders != 0) {
	  		pthread_cond_wait(&gWritePhase, &gSharedMemoryLock);
	  	}
	  	gReaders = -1;
	  	numReaders = gReaders;
	  pthread_mutex_unlock(&gSharedMemoryLock);

	  // Write data
	  fprintf(stdout, "[w%d] writing %u* [readers: %2d]\n", id, ++gSharedValue, numReaders);

	  // Exit critical section
	  pthread_mutex_lock(&gSharedMemoryLock);
	  	gReaders = 0;
	  	if (gWaitingReaders > 0) {
	  		pthread_cond_broadcast(&gReadPhase);
	  	}
	  	else {
	  		// this will ensure not writer wait forever
	  		// why they do it this way as oppose to just broadcast from the reader?
	  		// no other writer will make progress until this one finishes.
	  		// Why is this important at all.
	  		// This part of the writer problem we whould ensure that only
	  		// one writer access the resource at a given time. Even if we have
	  		// several writer threads which could still make progress but not enough
	  		// to write the value
	  		pthread_cond_signal(&gWritePhase);
	  	}
	  pthread_mutex_unlock(&gSharedMemoryLock);
  }	

  pthread_exit(0);	  
}
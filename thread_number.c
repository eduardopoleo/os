#include <stdio.h>
#include <pthread.h>
#define NUM_THREAD 4

// the input of this function has to be a
// classless pointer.
void *threadFunc(void *pArg) {
	// important. This variables are private to the
	// thread. Belong to the thread stack.
	int *p = (int*)pArg; // convert it to a pointer of type int
	int myNum = *p; // de reference it.

	printf("Thread number %d\n", myNum);
	return 0;
}

int main(void) {
	// this is global and will be used by all threads
	int i;
	int tNum[NUM_THREAD];

	pthread_t tid[NUM_THREAD];

	for(i=0; i < NUM_THREAD; i++) {
		tNum[i] = i;
		// the last are the arguments to pass to the 
		// * function to be ran by the thread.
		// with this we will ensure that all number will show up.
		pthread_create(&tid[i], NULL, threadFunc, &tNum[i]);
	}

	for(i=0; i < NUM_THREAD; i++) {
		pthread_join(tid[i], NULL);
	}

	return 0;
}

// This will not print the threads in order necesarily
// the thread execution order does not need to correlate
// with the creation order.
// Now this algorithm is faulty. We are passing &i which is a
// globally available variable and changes even if we are within
// the scope fo the threadFunc for the previous iteration.
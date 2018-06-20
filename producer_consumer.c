#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUF_SIZE 3

int buffer[BUF_SIZE];
int add = 0; // place to add the next element
int rem = 0; // place to remove the next element
int num = 0; // elements in the buffer

// It behave like a queue some how:
// add: tail
// rem: head

// this must be a standard c constant?
// mutex lock for buffer
// PTHREAD_MUTEX_INTIALIZER has init (itializes with defaults)
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

// consumer wait condition
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER;
// producer wait condition
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER;

// This all looks nice but we only have 2 threads..
// so I do not think we'll need any of the safety stuff?
// actually we do if the first thread runs faster it'll 
// need to stop then the producer will fill the buffer 
// cuz consumer is waiting.
void *producer (void *param) {
	int i;

	for (i = 0; i <= 20; i++) {
		// this does not automatically locks the path
		// it does it if the pthred_cond_wait has been called.
		// if the buffer if full there's no point in having
		// extra threads going at it
		pthread_mutex_lock(&m);
			if (num > BUF_SIZE) {
				// prevents overflow.
				exit(1);
			}

			while (num == BUF_SIZE) {
				// has to wait if the buffer is not full
				// but I think it will wait until woken up at this
				// probably it's a while loop so that it checks
				// again after woken up?

				// arg1 = condition to wait for
				// arg2 = the mutex we are locking.
				pthread_cond_wait (&c_prod, &m); // Why do we need to specify &m?
			}

			buffer[add] = i;
			// wouldn't this overflow? This is module!
			add = (add+1) % BUF_SIZE;
			num++;
		pthread_mutex_unlock(&m);

		// if there was a consumer thread waing to consume
		// elements from the buffer let's wake him up.
		pthread_cond_signal (&c_cons);
		printf("Producer: inserted %d\n", i); fflush(stdout);
	}

	printf("Producer quitting\n");
	return 0;
}

void *consumer (void *param) {
	int i;
	// this will nerver exit
	while(1) {
		pthread_mutex_lock (&m);
			if (num < 0) {
				exit(1);
			}

			while (num == 0) {
				pthread_cond_wait(&c_cons, &m);
				// the while loop is to reverify that theres
				// data the while loop is NOT what wakes up the 
				// thread.
			}

			i = buffer[rem];
			// 
			rem = (rem+1) % BUF_SIZE;
			num--;
		pthread_mutex_unlock(&m);

		// if theres a producer waiting to add wake him up
		pthread_cond_signal(&c_prod);
		printf("Consume value %d\n", i); fflush(stdout);
	}

	printf("Consumer quitting\n");
	return 0;
}


int main(int argc, char *argv[]) {
	// create the threads
	pthread_t tid1, tid2;
	int i;

	if (pthread_create(&tid2, NULL, consumer, NULL) != 0) {
		fprintf(stderr, "Unable to create consumer thread\n");
		exit(1);
	}

	if (pthread_create(&tid1, NULL, producer, NULL) != 0) {
		fprintf(stderr, "Unable to create producer thread\n");
		exit(1);
	}
	// takes thr thread, attrs of the thread, function to be run
	// and arguments for the function

	// joins all threads to the main thread
	// join basically waits for the children to execute to do it;s thing

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	printf("Parent thread quitting\n");
}
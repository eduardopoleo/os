#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUF_SIZE 3
#define MESSAGES 20

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumer_cond = PTHREAD_COND_INITIALIZER;

int buffer[BUF_SIZE];
int add = 0; // we need a global state to tell where to add 
int rem = 0; // global state to keep track of where to remove
int num = 0;

void *producer(void *args) {
	int index; 
	for (int i = 0; i < MESSAGES; i++) {
		pthread_mutex_lock(&m);
			while(num == BUF_SIZE) {
				pthread_cond_wait(&producer_cond, &m);
			}

			buffer[add] = i;
			add = (add + 1) % BUF_SIZE;
			num++;

			printf("PRODUCING: location: %d, valude %d\n", add, i);

			if (num != 0) {
				pthread_cond_signal(&consumer_cond);
			}
		pthread_mutex_unlock(&m);
	}

	pthread_exit(0);
}

void *consumer(void *args) {
	int i;

	while (1) {
		pthread_mutex_lock(&m);
			while(num == 0) {
				pthread_cond_wait(&consumer_cond, &m);
			}

			i = buffer[rem];
			rem = (rem + 1) % BUF_SIZE;
			num--;

			printf("CONSUMING location: %d, value: %d\n", rem, i);

			if(num < BUF_SIZE) {
				pthread_cond_signal(&consumer_cond);
			}
		pthread_mutex_unlock(&m);
	}

	pthread_exit(0);
}

int main(int argc, char **argv) {
	pthread_t t1, t2;

	pthread_create(&t1, NULL, producer, NULL);
	pthread_create(&t2, NULL, consumer, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}
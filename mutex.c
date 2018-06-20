// Mutex (as in mutually exclusive)
// solve the problem of mutual threads changing a share state
// at the same time.

#include <stdio.h>
#include <pthread.h>
#define NUM_THREAD 4

pthread_mutex_t aMutex; // create a mutext data s.

// the lock takes a mutex reference
// int pthread_mutex_lock(pthread_mutex_t *mutex)
// int pthread_mutex_unlock(pthread_mutex_t *mutex)
// Usage example

list<int> my_list;


pthread_mutex_t m;
void pthread safe_inset(int i) {
	pthread_mutex_lock(m);
	// this is the critical operation
	// and it's currently locked.
	my_list.insert(i); 
	pthread_mutex_unlock(m);
}

int pthread_mutex_init(
	pthread_mutex_t, *mutex,
	conts pthread_mutexattr_t *attr)
){}

int pthread_mutex_trylock(pthread_mutex_t *mutex){}
int pthread_mutex_destroy(pthread_mutex_t, *mutex){}

// condition variable used when locking mutexes
pthread_cond_t aCond;
// Initializes the data structure 
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond){}


// MAkes the thread wait on the condition
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex){}
// singnals the waiting thread that it can take over the mutex
int pthread_cond_signal(pthread_cond_t, *cond){}
// broadcast all other threads that we are done
int pthread_cond_broadcast(pthread_cond_t *cond) {}

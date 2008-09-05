#include <pthread.h>
#include <stdio.h>

int main(void) {
pthread_mutex_t ptl_lq_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_trylock(&ptl_lq_mutex);
	printf("I am working, really\n");	
	return 0;
}

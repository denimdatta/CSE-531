#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/************************************************/
// thread.h code
pthread_t start_thread(void *func, int *arg) {
	pthread_t thread_id;
	int rc;
	printf("In main: creating thread\n");
	rc = pthread_create(&thread_id, NULL, func, arg);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
	return(thread_id);
}

// sem.h code
typedef struct semaphore_t {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int count;
} semaphore_t;

void init_sem(semaphore_t *s, int i) {   
    s->count = i;
    pthread_mutex_init(&(s->mutex), NULL);
    pthread_cond_init(&(s->cond), NULL);
}

/*
 * The P routine decrements the semaphore, and if the value is less than
 * zero then blocks the process 
 */
void P(semaphore_t *sem) {   
	pthread_mutex_lock (&(sem->mutex)); 
	sem->count--;
	if (sem->count < 0) 
		pthread_cond_wait(&(sem->cond), &(sem->mutex));
	pthread_mutex_unlock (&(sem->mutex)); 
}

/*
 * The V routine increments the semaphore, and if the value is 0 or
 * negative, wakes up a process and yields
 */
void V(semaphore_t * sem) {   
	pthread_mutex_lock (&(sem->mutex)); 
	sem->count++;
	if (sem->count <= 0) {
		pthread_cond_signal(&(sem->cond));
	}
	pthread_mutex_unlock (&(sem->mutex));
	pthread_yield();
}
/************************************************/

// Main Project code starts here

semaphore_t mutexT1,mutexT2,mutexT3,mutexM;

void function_1(int *arg)
{
    while (1) {
		P(&mutexT1);
		(*arg)++;
		//printf("[ child1 ] : incremented to : %d\n", *arg);
		V(&mutexM);
    }
}

void function_2(int *arg)
{
    while (1) {
		P(&mutexT2);
		(*arg)++;
		//printf("[ child2 ] : incremented to : %d\n", *arg);
		V(&mutexM);
    }
}    

void function_3(int *arg)
{
    while (1) {
		P(&mutexT3);
		(*arg)++;
		//printf("[ child3 ] : incremented to : %d\n", *arg);
		V(&mutexM);
    }
}

int main()
{
	init_sem(&mutexT1, 1);
	init_sem(&mutexT2, 1);
	init_sem(&mutexT3, 1);
	init_sem(&mutexM, 0);
	
	
	int array[3] = {0,0,0};
	
    start_thread(function_1, &(array[0]));
    start_thread(function_2, &(array[1]));
    start_thread(function_3, &(array[2]));

    while(1) {
		P(&mutexM);
		P(&mutexM);
		P(&mutexM);
		printf("[ MAIN ] : Array Elements : {%d , %d , %d}\n", array[0], array[1], array[2]);
		sleep(1);
		V(&mutexT1);
		V(&mutexT2);
		V(&mutexT3);
	}

    return 0;
}

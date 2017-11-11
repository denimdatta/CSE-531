
#ifndef SEM_H
#define SEM_H

#include <stdlib.h>
#include <ucontext.h>
#include "q.h"
#include "threads.h"

typedef struct sem {
  int value;
  struct q_element *blocked;
} Semaphore_t;


Semaphore_t createSem(int value) 
{
	Semaphore_t *sem = malloc(sizeof(Semaphore_t));
	sem->value = value;
	sem->blocked = new_queue();
	
	return *sem;
}

void P(Semaphore_t *sem)
{
  sem->value -= 1;
  if (sem->value < 0) {
    struct q_element *element = new_item();
    element->tcb = current_thread;
    add_queue(sem->blocked, element);
    struct q_element *next = del_queue(ready_queue);
    current_thread = next->tcb;
    swapcontext(&(element->tcb->context), &(current_thread->context));
  }
}

void V(Semaphore_t *sem)
{
  sem->value += 1;
  if (sem->value <= 0) {
    struct q_element *element = del_queue(sem->blocked);
    add_queue(ready_queue, element);
  }
  yield();
}

#endif

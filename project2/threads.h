
#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>
#include <ucontext.h>
#include "q.h"
#include "tcb.h"

#define STACK_SIZE 8192

int thread_id = 1;

tcb_t *ready_queue, *current_thread;

void start_thread(void (*function)(void))
{
    char *stack = malloc(STACK_SIZE);
    tcb_t *tcb = new_item();
    init_tcb(tcb, function, stack, STACK_SIZE);

    if (!ready_queue)
        ready_queue = new_queue();
	
	tcb->thread_id = thread_id;
	thread_id++;
    add_queue(ready_queue, tcb);
}

void run()
{
    if (!ready_queue || current_thread)
        return;

    current_thread = del_queue(ready_queue);
	ucontext_t parent;
	getcontext(&parent);
    
	swapcontext(&parent, &(current_thread->context));
}

void yield()
{
    if (!ready_queue || !current_thread)
        return;
	
	tcb_t * prev_thread;
	add_queue(ready_queue, current_thread);
	prev_thread = current_thread;
	current_thread = del_queue(ready_queue);
    swapcontext(&(prev_thread->context), &(current_thread->context));    
}

#endif

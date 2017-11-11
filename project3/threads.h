
#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>
#include <ucontext.h>
#include "q.h"
#include "tcb.h"

#define STACK_SIZE 8192

struct q_element *ready_queue;
tcb_t *current_thread;

void start_thread(void (*function)(void))
{
    char *stack = malloc(STACK_SIZE);
    tcb_t *tcb = malloc(sizeof(tcb_t));
    init_tcb(tcb, function, stack, STACK_SIZE);

    if (!ready_queue)
        ready_queue = new_queue();

    struct q_element *element = new_item();
    element->tcb = tcb;
    add_queue(ready_queue, element);
}

void run()
{
    if (!ready_queue || current_thread)
        return;

    struct q_element *element = del_queue(ready_queue);
    if (element) {
        current_thread = element->tcb;
        setcontext(&(current_thread->context));
    }
}

void yield()
{
    if (!ready_queue || !current_thread)
        return;

    struct q_element *element = new_item();
    element->tcb = current_thread;
    add_queue(ready_queue, element);
    current_thread = del_queue(ready_queue)->tcb;
    swapcontext(&(element->tcb->context), &(current_thread->context));
}

#endif

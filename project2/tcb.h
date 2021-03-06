
#ifndef TCB_H
#define TCB_H

#include <string.h>
#include <ucontext.h>

typedef struct tcb_t {
    int thread_id;
	ucontext_t context;
    struct tcb_t *prev;
    struct tcb_t *next;
} tcb_t;

void init_tcb (tcb_t *tcb, void (*function)(void), void *stack, int stack_size)
{
    static int unique_thread_id;

    memset(tcb, 0, sizeof(tcb_t));
    getcontext(&tcb->context);
    tcb->context.uc_stack.ss_sp = stack;
    tcb->context.uc_stack.ss_size = (size_t) stack_size;
    tcb->thread_id = unique_thread_id++;
    makecontext(&tcb->context, function, 0);
}

#endif

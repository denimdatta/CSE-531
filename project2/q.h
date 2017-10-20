
#ifndef Q_H
#define Q_H

#include <stdlib.h>
#include "tcb.h"

struct tcb_t * new_item(void)
{
    struct tcb_t *item = calloc(1, sizeof(struct tcb_t));
    return item;
}

struct tcb_t * new_queue(void)
{
    struct tcb_t *head = calloc(1, sizeof(struct tcb_t));
    head->prev = head;
    head->next = head;
    return head;
}

void add_queue(struct tcb_t *head, struct tcb_t *element)
{
    struct tcb_t *last = head->prev;
    last->next = element;
    element->next = head;
    element->prev = last;
    head->prev = element;
}

struct tcb_t * del_queue(struct tcb_t *head)
{
    struct tcb_t *result = NULL;
    if (head != head->next) {
        result = head->next;
        head->next = head->next->next;
        head->next->prev = head;
    }
    return result;
}

#endif

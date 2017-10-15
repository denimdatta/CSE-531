
#ifndef Q_H
#define Q_H

#include <stdlib.h>
#include "tcb.h"

struct q_element {
    tcb_t *tcb;
    struct q_element *prev;
    struct q_element *next;
};

struct q_element * new_item(void)
{
    struct q_element *item = calloc(1, sizeof(struct q_element));
    return item;
}

struct q_element * new_queue(void)
{
    struct q_element *head = calloc(1, sizeof(struct q_element));
    head->prev = head;
    head->next = head;
    return head;
}

void add_queue(struct q_element *head, struct q_element *element)
{
    struct q_element *last = head->prev;
    last->next = element;
    element->next = head;
    element->prev = last;
    head->prev = element;
}

struct q_element * del_queue(struct q_element *head)
{
    struct q_element *result = NULL;
    if (head != head->next) {
        result = head->next;
        head->next = head->next->next;
        head->next->prev = head;
    }
    return result;
}

#endif

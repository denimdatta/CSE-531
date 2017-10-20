
#include <stdio.h>
#include <unistd.h>
#include "threads.h"

int increment_val = 0;

void increment(void);
void twice(void);
void triple(void);

int main(void)
{
    puts("Starting thread \"increment\"...");
    start_thread(increment);
    puts("Starting thread \"twice\"...");
    start_thread(twice);
    puts("Starting thread \"triple\"...");
    start_thread(triple);
    puts("Running threads...");
    run();
    return 0;
}

void increment(void)
{
    int val = 0;
    while (1) {
        val += 1;
        printf("Thread(%d): %d : %d  (increment)\n", current_thread->thread_id, increment_val, val);
		increment_val++;
        usleep(1 * 1000000);
        yield();
    }
}

void twice(void)
{
    int val = 0;
    while (1) {
        val += 2;
        printf("Thread(%d): %d : %d  (twice)\n", current_thread->thread_id, increment_val, val);
        increment_val++;
		usleep(1 * 1000000);
        yield();
    }
}

void triple(void)
{
    int val = 0;
    while (1) {
        val += 3;
        printf("Thread(%d): %d : %d  (triple)\n", current_thread->thread_id, increment_val, val);
        increment_val++;
		usleep(1 * 1000000);
        yield();
    }
}

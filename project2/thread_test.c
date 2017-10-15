
#include <stdio.h>
#include <unistd.h>
#include "threads.h"

int increment_val = 0;

void increment(void);
void triple(void);
void spawn_threads(void);

int main(void)
{
    puts("Starting thread \"increment\"...");
    start_thread(increment);
    puts("Starting thread \"triple\"...");
    start_thread(triple);
    puts("Starting thread \"spawn_threads\"...");
    start_thread(spawn_threads);
    puts("Running threads...");
    run();
    return 0;
}

void increment(void)
{
    while (1) {
        increment_val++;
        printf("Thread(%d): %d\n", current_thread->thread_id, increment_val);
        usleep(1 * 1000000);
        yield();
    }
}

void triple(void)
{
    int val = 1;
    while (1) {
        val *= 3;
        printf("Thread(%d): %d\n", current_thread->thread_id, val);
        usleep(1 * 1000000);
        yield();
    }
}

void spawn_threads(void)
{
    while (1) {
        start_thread(increment);
        printf("Thread(%d): Spawning new \"increment\" thread...\n", current_thread->thread_id);
        usleep(1 * 1000000);
        yield();
    }
}

#include "threads.h"

Threads *create_threads_struct(int max_size)
{
    Threads *t;
    /* 
    ** This odd malloc is due to how flexible/dynamic array members work
    ** inside a struct. You don't malloc the array separately like this:
    ** q->queue = malloc(sizeof(Process*) * BASE_QUEUE_SIZE);
    ** but instead just malloc the Queue the size of both the size of the
    ** struct AND the size of the array.
    */
    t = malloc(sizeof(Threads) + sizeof(pthread_t) * max_size);
    t->start = 0;
    t->num_items = 0;
    t->max_size = max_size;
    return t;
}

int get_next_index(Threads *t)
{

    if (t->num_items == t->max_size) {
        return -1;
    }

    int next_index = (t->start + t->num_items) % t->max_size;
    t->num_items += 1;

    return next_index;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct Threads_s {
    /*
    ** The array is used cyclically. TODO explain some more.
    */
    int start;
    int num_items;
    int max_size;
    pthread_t t[];
} Threads;

Threads *create_threads_struct(int max_size);
int get_next_index(Threads *t);
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

typedef struct Instance_s {
    pthread_t t;
    int s;
    int turn;
    char *ip4;
} Instance;

typedef struct Instances_s {
    /*
    ** The array is used cyclically. TODO explain some more.
    */
    int start;
    int num_items;
    int max_size;

    // TODO explain why using this.
    Instance *i[];
} Instances;

Instances *create_instances_struct(int max_size);
Instance *new_instance(Instances *instances, int sock_id, char *ip4, pthread_t thread_id);
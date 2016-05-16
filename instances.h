#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

typedef struct Instance_s {
    pthread_t t;
    int s;
    int turn;
    char *ip4;
    char *code;
} Instance;

typedef struct Instances_s {
    /*
    ** The array is used cyclically. TODO explain some more.
    */
    int num_items;
    int max_size;
    // TODO explain why using this.
    Instance *i[];
} Instances;

Instances *create_instances_struct(int max_size);
Instance *new_instance(Instances *insts, int sock_id, char *ip4, pthread_t thread_id);
void remove_instance(Instances *insts, pthread_t thread_id);
void print_instances(Instances *insts);
Instance *get_instance(Instances *insts, pthread_t thread_id);
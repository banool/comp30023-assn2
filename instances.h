// Daniel Porteous porteousd

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

typedef struct StateInfo_s {
    /*
    ** The array is used cyclically. TODO explain some more.
    */
    int num_items;
    int max_size;
    // TODO explain why using this.
    Instance *instances[];
} StateInfo;

StateInfo *create_state_info_struct(int max_size);
Instance *new_instance(StateInfo *state_info, int sock_id, char *ip4, pthread_t thread_id);
void remove_instance(StateInfo *state_info, pthread_t thread_id);
void print_instances(StateInfo *state_info);
Instance *get_instance(StateInfo *state_info, pthread_t thread_id);
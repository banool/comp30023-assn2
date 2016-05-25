// Daniel Porteous porteousd

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>

// Struct representing a single game instance.
typedef struct Instance_s {
    pthread_t t;
    int s;
    char *ip4;
    int turn;
    char *code;
} Instance;

// Holds info about the entire server state, with a struct holding
// info for each game currently running.
typedef struct StateInfo_s {
    int num_items;
    int max_size;
    // An array of pointers to structs. This is a flexible
    // array member and will require a special malloc.
    Instance *instances[];
} StateInfo;

StateInfo *create_state_info_struct(int max_size);
Instance *new_instance(StateInfo *state_info, int sock_id, char *ip4, pthread_t thread_id);
void remove_instance(StateInfo *state_info, pthread_t thread_id);
void print_instances(StateInfo *state_info);
Instance *get_instance(StateInfo *state_info, pthread_t thread_id);
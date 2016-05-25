// Daniel Porteous porteousd

#include "instances.h"

extern pthread_mutex_t lock;

// Creates the StateInfo struct which tracks the info of each individual game.
StateInfo *create_state_info_struct(int max_size)
{
    StateInfo *state_info;
    /* 
    ** This odd malloc is due to how flexible/dynamic array members work
    ** inside a struct. You don't malloc the array separately but
    ** instead just malloc the Queue the size of both the size of the
    ** struct AND the size of the array in the one malloc.
    */
    state_info = malloc(sizeof(StateInfo) + sizeof(Instance*) * max_size);
    assert(state_info);

    // Setting all the pointers in the array of pointers to an Instance
    // to NULL. We do this because when we insert an item in the array,
    // we find the first one that's not NULL. We need to therefore make
    // sure that each one is set to NULL first and isn't just garbage.
    for (int x = 0; x < max_size; x++) {
        state_info->instances[x] = NULL;
    }

    state_info->num_items = 0;
    state_info->max_size = max_size;
    return state_info;
}

// Create a new_instance, insert it into the StateInfo struct and return it.
Instance *new_instance(StateInfo *state_info, int sock_id, char *ip4,
    pthread_t thread_id)
{
    // If we are at max players, reject the request to make a new_instance.
    if (state_info->num_items == state_info->max_size) {
        return NULL;
    }

    Instance *i = malloc(sizeof(Instance));
    assert(i);

    i->s =   sock_id;
    i->t =   thread_id;
    i->ip4 = malloc(sizeof(char) * strlen(ip4));
    strncpy(i->ip4, ip4, strlen(ip4)+1);
    i->ip4[strlen(ip4)] = '\0';
    i->turn = 1;

    // Use a mutex lock before adding the new instance to the array so we
    // don't accidentally accidentally write to the same index twice.
    pthread_mutex_lock(&lock);
    
    // Iterate through the array until we find an empty spot for the new game.
    int index = 0;
    while (state_info->instances[index] != NULL) {
        index++;
    }

    state_info->instances[index] = i;
    state_info->num_items += 1;

    pthread_mutex_unlock(&lock);
    return i;
}

// Removes an instance by finding its thread number.
// Should be ok because thread numbers are unique.
void remove_instance(StateInfo *state_info, pthread_t thread_id)
{
    for (int x = 0; x < state_info->max_size; x++) {
        if (state_info->instances[x] != NULL) {
            if (state_info->instances[x]->t == thread_id) {
                // Use a mutex lock while removing and changing the recorded
                // number of items to avoid clashes.
                pthread_mutex_lock(&lock);
                free(state_info->instances[x]);
                state_info->instances[x] = NULL;
                state_info->num_items -= 1;
                pthread_mutex_unlock(&lock);
            }
        }
    }
}

// Print all the instances. Purely diagnostic.
void print_instances(StateInfo *state_info)
{
    for (int x = 0; x < state_info->max_size; x++) {
        if (state_info->instances[x] != NULL) {
            printf("index: %d: thread_id: %d sock_id: %d ip4: %s\n", x, 
                (int)state_info->instances[x]->t, state_info->instances[x]->s, 
                state_info->instances[x]->ip4);
        }
    }
}

/*
** Retrieves an instance from StateInfo by looking for its unique thread id.
** We use this so we can find the appropriate instance when a new thread
** is spawned, considering we can't pass the instance through to the worker
** function of pthread_create because we need to pass StateInfo.
*/
Instance *get_instance(StateInfo *state_info, pthread_t thread_id)
{
    for (int x = 0; x < state_info->max_size; x++) {
        if (state_info->instances[x] != NULL) {
            if (state_info->instances[x]->t == thread_id) {
                return state_info->instances[x];
            }
        }
    }
    // We should never get here as we only ever pass valid thread_ids.
    return NULL;
}







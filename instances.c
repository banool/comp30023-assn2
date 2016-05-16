#include "instances.h"

Instances *create_instances_struct(int max_size)
{
    Instances *insts;
    /* 
    ** This odd malloc is due to how flexible/dynamic array members work
    ** inside a struct. You don't malloc the array separately like this:
    ** q->queue = malloc(sizeof(Process*) * BASE_QUEUE_SIZE);
    ** but instead just malloc the Queue the size of both the size of the
    ** struct AND the size of the array.
    */
    insts = malloc(sizeof(Instances) + sizeof(Instance*) * max_size);
    assert(insts);

    // Setting all the pointers in the array of pointers to an Instance
    // to NULL. We do this because when we insert an item in the array,
    // we find the first one that's not NULL. We need to therefore make
    // sure that each one is set to NULL first and isn't just garbage.
    for (int x = 0; x < max_size; x++) {
        insts->i[x] = NULL;
    }

    insts->start = 0;
    insts->num_items = 0;
    insts->max_size = max_size;
    return insts;
}

/*
int get_next_index(Instances *i)
{

    if (i->num_items == i->max_size) {
        return -1;
    }

    
    i->num_items += 1;

    return next_index;
}
*/

Instance *new_instance(Instances *insts, int sock_id, char *ip4, pthread_t thread_id)
{
    //TODO you need mutext locks at points in this.
    // TODO explain
    if (insts->num_items == insts->max_size) {
        return NULL;
    }

    Instance *i = malloc(sizeof(Instance));
    assert(i);

    i->s = sock_id;
    i->t = thread_id;
    i->ip4 = malloc(sizeof(char) * strlen(ip4));
    strncpy(i->ip4, ip4, strlen(ip4));
    i->turn = 1;

    // TODO explain this line
    int index = 0;
    while (insts->i[index] != NULL) {
        index++;
    }

    insts->i[index] = i;

    insts->num_items += 1;
    return i;
}

// Removes an instance by finding its thread number.
// Should be ok because thread numbers are unique.
void remove_instance(Instances *insts, pthread_t thread_id) {
    for (int x = 0; x < insts->max_size; x++) {
        if (insts->i[x] != NULL) {
            if (insts->i[x]->t == thread_id) {
                free(insts->i[x]);
                insts->i[x] = NULL;
                insts->num_items -= 1;
            }
        }
    }
}

void print_instances(Instances *insts) {
    for (int x = 0; x < insts->max_size; x++) {
        if (insts->i[x] != NULL) {
            printf("index: %d: thread_id: %d sock_id: %d ip4: %s\n", x, insts->i[x]->t, insts->i[x]->s, insts->i[x]->ip4);
        }
    }
}







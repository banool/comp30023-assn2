#include "instances.h"

Instances *create_instances_struct(int max_size)
{
    Instances *i;
    /* 
    ** This odd malloc is due to how flexible/dynamic array members work
    ** inside a struct. You don't malloc the array separately like this:
    ** q->queue = malloc(sizeof(Process*) * BASE_QUEUE_SIZE);
    ** but instead just malloc the Queue the size of both the size of the
    ** struct AND the size of the array.
    */
    i = malloc(sizeof(Instances) + sizeof(Instance*) * max_size);
    assert(i);
    i->start = 0;
    i->num_items = 0;
    i->max_size = max_size;
    return i;
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
    int next_index = (insts->start + insts->num_items) % insts->max_size;
    insts->i[next_index] = i;

    insts->num_items += 1;
    return i;
}
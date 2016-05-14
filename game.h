#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "threads.h"

#define CODE_LENGTH 4
#define MAX_PLAYERS 20

int cmp_codes(char *guess, char *correct, int *b, int *m);
int create_game(int sock_id, Threads *threads);
void *work_function(int sock_id);
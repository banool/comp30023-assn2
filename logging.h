// Daniel Porteous porteousd

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define LOG_TIME_LEN 21
// 128 minus 18 characters for the time.
#define LOG_MSG_LEN 110

void write_log(char *inp);
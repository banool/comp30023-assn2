#include "logging.h"

extern pthread_mutex_t lock;

void write_log(char *inp) {

	char curr_time[LOG_TIME_LEN];

	// Time made using code from:
	// https://goo.gl/7r62mD
	time_t timer;
	struct tm* tm_info;

	time(&timer);
    tm_info = localtime(&timer);

    strftime(curr_time, LOG_TIME_LEN, "%d %m %Y %H:%M:%S", tm_info);

    //pthread_mutex_lock(&lock);
	printf("[%s] %s", curr_time, inp);
	//pthread_mutex_unlock(&lock);

    memset(inp, '\0', LOG_MSG_LEN);

}
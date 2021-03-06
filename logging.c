// Daniel Porteous porteousd

#include "logging.h"

extern pthread_mutex_t lock;
extern FILE *log_f;

void write_log(char *inp) {

	pthread_mutex_lock(&lock);
	char curr_time[LOG_TIME_LEN];

	// Time made using code from:
	// https://goo.gl/7r62mD
	time_t timer;
	struct tm* tm_info;

	time(&timer);
    tm_info = localtime(&timer);

    strftime(curr_time, LOG_TIME_LEN, "%d %m %Y %H:%M:%S", tm_info);

	fprintf(log_f, "[%s] %s", curr_time, inp);
        fflush(log_f);

    memset(inp, '\0', LOG_MSG_LEN);
    pthread_mutex_unlock(&lock);

}

void write_log_raw(char *inp) {

	pthread_mutex_lock(&lock);
	fprintf(log_f, "%s", inp);
	pthread_mutex_unlock(&lock);

	memset(inp, '\0', LOG_MSG_LEN);
}

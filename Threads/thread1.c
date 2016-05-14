/*************************************
 
 Demo for pthread commands
 compile: gcc threadX.c -o threadX -lpthread
 
***************************************/

#include <stdio.h>
#include <pthread.h>

void *entry_point(void *param);  /* the work_function */

int main(int args, char **argv) 
{
	pthread_t tid; /* thread identifier */
   
	/* create the thread */
	pthread_create(&tid, NULL, entry_point, NULL);
	
	/* wait for thread to exit */ 
	pthread_join(tid, NULL);

	printf("Hello from first thread\n");   
	return 0;
}

void *entry_point(void *param) 
{
	printf("Hello from second thread\n");	
	return NULL;
}


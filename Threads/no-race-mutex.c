/* gcc norace.c -o norace -lpthread */

/* does this code work correctly? */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define ITERATIONS 1000000

void *runner(void *param);   /* thread doing the work */

int count = 0;
pthread_mutex_t lock;

int main(int argc, char **argv)
{
    pthread_t tid1, tid2;	
    int value;
	
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        exit(1);
    }	
	
    if(pthread_create(&tid1, NULL, runner, NULL))
    {
      printf("\n Error creating thread 1");
      exit(1);
    }

    if(pthread_create(&tid2, NULL, runner, NULL))
    {
      printf("\n Error creating thread 2");
      exit(1);
    }

    if(pthread_join(tid1, NULL))	/* wait for the thread 1 to finish */
    {
      printf("\n Error joining thread");
      exit(1);
    }

    if(pthread_join(tid2, NULL))        /* wait for the thread 2 to finish */
    {
      printf("\n Error joining thread");
      exit(1);
    }

    if (count < 2 * ITERATIONS) 
        printf("\n ** ERROR ** count is [%d], should be %d\n", count, 2*ITERATIONS);
    else
        printf("\n OK! count is [%d]\n", count);
  
    pthread_exit(NULL);
    pthread_mutex_destroy(&lock);
    
    return 0;
	
}

void *runner(void *param)   /* thread doing the work */
{
    pthread_mutex_lock(&lock);
	
    int i, temp;
    for(i = 0; i < ITERATIONS; i++)
    {
        temp = count;	    /* copy the global count locally */
        temp = temp + 1;    /* increment the local copy */
        count = temp;	    /* store the local value into the global count */ 
    }
	
   pthread_mutex_unlock(&lock);
}



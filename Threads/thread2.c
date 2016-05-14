#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N 2 

void *work_function(void *param);

int count=0;   

int main(int args, char **argv) 
{
    pthread_t tid[N];
    long int iterations = atoi(argv[1]);

    pthread_create(&tid[0], NULL, work_function, (void *) iterations);
    pthread_create(&tid[1], NULL, work_function, (void *) iterations);
    
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    if (count != iterations * 2)
	    printf("Error: %d\n",count);
    else
	    printf("Value as expected: count = %d\n", count);

    pthread_exit(NULL); 
}

void *work_function(void *param)
{
    long int max_iter = (long int) param;
    int i;
    for(i = 0; i < max_iter; i++)
		count++;
    
    pid_t pid = getpid();
    pthread_t id = pthread_self();
    printf("in thread: pid=%d and id=%u count=%d\n",
		(unsigned int) pid, (unsigned int) id, count);
	
    pthread_exit(NULL); 
}

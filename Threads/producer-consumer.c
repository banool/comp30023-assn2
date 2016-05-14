#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void *produce(void *);
void *consume(void *);

sem_t produced, consumed;
int n = 0;

int main(int argc, char **argv)
{
	pthread_t idprod, idcons;
	int loopcnt = 5;
	
	if (sem_init(&consumed, 0, 0) < 0) {
		perror("sem_init"); exit(1);
	}
    
	if (sem_init(&produced, 0, 1) < 0) {
		perror("sem_init"); exit(1);
	}
	
	if (pthread_create(&idprod, NULL, produce, (void *)loopcnt) != 0) {
		perror("pthread_create"); exit(1);
	}
	
	if (pthread_create(&idcons, NULL, consume, (void *)loopcnt) != 0) {
		perror("pthread_create"); exit(1);
	}
	
	pthread_join(idprod, NULL);
	pthread_join(idcons, NULL);
    
	sem_destroy(&produced);
	sem_destroy(&consumed);
}        

void *produce(void *arg)
{
	int i, loopcnt;
	loopcnt = (int) arg;
    
	for (i=0; i<loopcnt; i++) {
		sem_wait(&consumed);
		n++; 
		sem_post(&produced);
	}
}       

void *consume(void *arg)
{
	int i, loopcnt;
	loopcnt = (int) arg;
    
	for (i=0; i<loopcnt; i++) {
		sem_wait(&produced);
		printf("%d\n", n);
		sem_post(&consumed);
	}
}

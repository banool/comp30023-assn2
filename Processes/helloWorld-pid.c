/*************************************
 
 demo for ps commands
 
examine the PID for a running program
 
 **************************************/

#include <stdio.h>
#include <unistd.h>

int main(int args, char **argv)
{
	printf("Hello world\n");

	printf("PID = %d\n", getpid());
	printf("parent PID = %d\n", getppid());
	
	while(1)
        	sleep(1);
	return 0;
}

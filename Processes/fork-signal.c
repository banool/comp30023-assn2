
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
// #include <signal.h>

void signal_handler()
{
   printf("\nTime to stop.\n");
   exit(0);
}


int main(int agrc, char **argv)
{
	pid_t pid;

	/* fork a child process */

	pid = fork();

	if(pid < 0)  /* an error occurred */
	{
		printf("Fork failed\n");
		return 1;
	}

 	signal(SIGUSR1, signal_handler); 

	if(pid != 0) /* parent  process */
	{
		printf("Parent waiting...\n");
		sleep(1);
		kill(pid, SIGUSR1);	/* tell child to stop */	
	}
	else /* child process */
	{
		int i = 0;

		while (1) 
		{
      			i++;
      			printf("%d\n", i);
		}
	}

	return 0;
}



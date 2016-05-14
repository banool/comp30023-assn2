/* Silberscatz, Galvin,and Gagne. (2009) Fig 3.10 */

/*************************************
 
 demo fork() commands
 
 examine the PIDs
 
 
 **************************************/

#include <stdio.h>
#include <unistd.h>

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
	else if(pid == 0) /* child process */
	{
		printf("Child process\n\n");
		execlp("/bin/ls", "ls", NULL);
	}
	else /* parent process */
	{
		printf("\nParent process\n");
		/* parent waits for the child to complete */
		wait(NULL);

		printf("Child complete\n");
	}

	return 0;
}



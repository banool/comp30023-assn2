#include <stdio.h>
#include <stdlib.h>

int main(int args, char **argv)
{
	printf("location of code = %p\n",	(void *) main);
	printf("location of heap = %p\n",	(void *) malloc(1));
	int x = 3;
	printf("location of stack = %p\n",	(void *) &x);
	printf("value of x = %d\n",			 x);

        while(1) sleep(1);
	
	return 0;
}

/*************************************
 
 demo exec() commands

 **************************************/

#include <stdio.h>
#include <unistd.h>

int main(int agrc, char **argv)
{
	execlp("/bin/ls", "bin/ls", "-l", NULL); 
	
	return 0;
}



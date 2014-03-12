#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void child();

int main(){
	pid_t pid;

	pid = fork();
	if(pid<0)
		printf("fork error!\n");
	else if(pid==0)
		child();
	return 0;
}

void child(){
	execlp("ps", "ps", "-aux", NULL);
	exit(0);
	return;
}

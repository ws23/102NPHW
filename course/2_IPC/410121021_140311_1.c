#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

int fib(int);
void child();
void parent();

int fd1[2], fd2[2];

int main(){
	pid_t pid;
	char msg[10];

	if(pipe(fd1)==-1)
		printf("pipe fd1 error\n");
	if(pipe(fd2)==-1)
		printf("pipe fd2 error\n");

	pid = fork();

	if(pid==0)
		child();
	else if(pid>0)
		parent();
	else
		printf("fork error\n");

	return 0;
}

void child(){
	int n;
	char buf[20];
	printf("Child Process.\n");	
	while(1){
		printf("Input a number(<0 to exit): ");
		scanf("%d", &n);
		if(n<0)
			exit(0);
		close(fd1[0]);
		sprintf(buf, "%d", n);
		write(fd1[1], buf, 20);	
		close(fd2[1]);
		read(fd2[0], buf, 20);
		printf("fib(%d) = %s\n", n, buf);
	}
}

void parent(){
	char buf[20];
	int n;
	printf("Parent Process.\n");
	while(1){
		close(fd1[1]);
		read(fd1[0], buf, 20);
		n = atoi(buf);
		sprintf(buf, "%d", fib(n));
		close(fd2[0]);
		write(fd2[1], buf, 20);
	}
}

int fib(int x){
	static int i = 2, f[100] = {0, 1};
	if(i==2){
		for(;i<100;i++)
			f[i] = f[i-1]+f[i-2];
	}
	return f[x];
}

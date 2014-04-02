#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#define FIFO_SERVER "/tmp/fifoserver"

int fib(int);

int main(){
	char buff[20];
	int n, fd;

	while(1){
		fd = open(FIFO_SERVER, O_RDONLY);
		if(fd==-1)
			exit(0);
		read(fd, buff, 20);
		n = atoi(buff);
		printf("fib(%s) = %d\n", buff, fib(n));
		close(fd);
	}
	return 0;
}

int fib(int x){
	static i=2, f[100] = {0, 1};
	if(i==2){
		for(;i<100;i++)
			f[i] = f[i-1]+f[i-2];
	}
	return f[x];
}


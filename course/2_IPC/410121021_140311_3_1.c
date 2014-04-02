#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#define FIFO_SERVER "/tmp/fifoserver"

int main(){
	int  n, fd;
	char buff[20];

	mkfifo(FIFO_SERVER, 0644);

	while(1){
		fd = open(FIFO_SERVER, O_WRONLY);
		printf("Input a number(<0 to exit): ");
		scanf("%d", &n);
		if(n<0){
			unlink(FIFO_SERVER);
			exit(0);
		}
		sprintf(buff, "%d", n);
		write(fd, buff, 20);	

		close(fd);
	}
	return 0;
}


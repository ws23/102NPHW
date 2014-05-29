#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>                                                 
#include <sys/socket.h>
#define MAXLINE 1024

/*Calculate the fib(n)*/

int fib(int n){
	static i = 2, fib[100] = {0, 1}; 
	if(i==2)
		for(;i<100;i++)
			fib[i] = fib[i-1] + fib[i-2]; 
	return fib[n]; 
}

int main(int argc, char *argv[]){
	int listenfd, connfd, len, n; 
	struct sockaddr_in servaddr, clntaddr; 
	char buff[MAXLINE]; 

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("socket error\n"); 
		exit(1); 
	}

	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(1243); 
	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
		printf("bind error\n"); 
		exit(1); 
	} 

	listen(listenfd, 128); 

	while(1){
		len = sizeof(clntaddr); 	
		connfd = accept(listenfd, (struct sockaddr*)&clntaddr, &len); 
		printf("accept success\n"); 
		while(1){
			read(connfd, buff, sizeof(buff));
			n = atoi(buff); 
			n = ntohl(n);
			printf("get %d\n", n);
			if(n<0)
				break;
			n = fib(n); 
			sprintf(buff, "%d", htonl(n)); 
			write(connfd, buff, sizeof(buff));
			printf("send %d\n", n);
			read(connfd, buff, sizeof(buff)); 
		}
		close(connfd); 
	}
	close(listenfd); 
	return 0; 
}

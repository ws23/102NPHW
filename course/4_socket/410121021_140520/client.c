#include <netinet/in.h>                                                 
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAXLINE 1024

/* Input a value, send to server, show the result from server */

int main(int argc, char *argv[]){
	int sockfd, n; 
	char recvline[MAXLINE+1];
	struct sockaddr_in servaddr; 

	if(argc!=2){
		printf("usage: %s <IP Address>\n", argv[0]); 
		exit(0); 
	}
		if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
			printf("socket error\n"); 
			exit(1); 
		}

		bzero(&servaddr, sizeof(servaddr)); 
		servaddr.sin_family = AF_INET; 
		servaddr.sin_port = htons(1243); 
		if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0){
			printf("inet_pton error for %s\n", argv[1]); 
			exit(1); 
		}	
		if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
			printf("connect error\n"); 
			exit(1); 
		}


	while(1){
		printf("Input a value to calculate(<0 to exit): \n> "); 
		scanf("%d", &n);
		sprintf(recvline, "%d", htonl(n)); 
		write(sockfd, recvline, sizeof(recvline)); 
		printf("send %d\n", n);
		if(n<0)
			break;
		read(sockfd, recvline, sizeof(recvline)); 
		n = atoi(recvline); 
		n = ntohl(n); 
		printf("%d\n", n); 
	}
	close(sockfd); 
	exit(0); 
}

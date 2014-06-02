#include <netinet/in.h>                                                 
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define MAXLINE 1024

/* Input a value, send to server, show the result from server */

int main(int argc, char *argv[]){
	int sockfd, n, tmp; 
	socklen_t len; 
	char buff[MAXLINE+1];
	struct sockaddr_in servaddr; 

	if(argc!=2){
		printf("usage: %s <IP Address>\n", argv[0]); 
		exit(0); 
	}
	
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(1243); 
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0){
		printf("inet_pton error for %s\n", argv[1]); 
		exit(1); 
	}	

	while(1){
		printf("Input a value to calculate(<0 to exit): \n> "); 
		scanf("%d", &n);
		sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
		len = sizeof(servaddr); 
		memset(buff, 0, sizeof(buff)); 
		sprintf(buff, "%d", htonl(n)); 
		printf("send buff = %s\n", buff); 
		sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr*)&servaddr, len); 
		printf("send %d\n", n);
		if(n<0)
			break;
		memset(buff, 0, sizeof(buff)); 
		tmp = recvfrom(sockfd, buff, MAXLINE, 0, NULL, NULL); 
		printf("get buff = %s\n", buff); 
		n = atoi(buff); 
		n = ntohl(n); 
		printf("%d\n", n); 
	}
	close(sockfd); 
	exit(0); 
}

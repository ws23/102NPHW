#include "five.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define SERV_COMM_PORT 15926
#define MAXLINE 1024

int commfd; 

int comm_socket_setup(char *servIP, int servPort){
	int len; 
	struct sockaddr_in comm_socket; 
	if((commfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("ERROR!!commfd cannot open!!\n"); 
		return -1; 
	}
	printf("communication fd %d\n", commfd); 
	bzero(&comm_socket, sizeof(comm_socket)); 
	comm_socket.sin_family = AF_INET; 
	comm_socket.sin_port = htons(servPort); 
	if(inet_pton(AF_INET, servIP, &comm_socket.sin_addr)<=0){
		perror("ERROR!!inet_pton error!!\n"); 
		return -1; 
	}
	if(connect(commfd, (struct sockaddr*)&comm_socket, sizeof(comm_socket))<0){
		perror("ERROR!!commfd cannot connect!n"); 
		return -1; 
	}
	if(getsockname(commfd, (struct sockaddr*)&comm_socket, &len)<0){
		perror("ERROR!!commfd cannot use getsockbyname\n"); 
		//return -1; 
	}
	printf("local IP is %s:%d\n", inet_ntoa(comm_socket.sin_addr), ntohs(comm_socket.sin_port)); 
	return 0; 
}

int main(int argc, char *argv[]){
	char msg[MAXLINE], *IP; 
	int ID; 
	struct hostent *host; 
	fd_set socket_set; 
	if(argc!=2){
		printf("usage: %s <IPaddress>\n", argv[0]); 
		exit(0); 
	}
	host = gethostbyname(argv[1]);
	printf("hostname: %s, hostIP: %s\n", host->h_name, inet_ntoa(*(struct in_addr*)host->h_addr)); 
	if(comm_socket_setup(inet_ntoa(*(struct in_addr*)host->h_addr), SERV_COMM_PORT)<0)
		exit(0); 
	do{
		bzero(msg, MAXLINE); 
		printf("Input the message\n"); 
		FD_ZERO(&socket_set); 
		FD_SET(commfd, &socket_set); 
		FD_SET(0, &socket_set); 
		if(select(10, &socket_set, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)0)>=0){
			if(FD_ISSET(0, &socket_set)){
				scanf("%s", msg); 
				write(commfd, msg, MAXLINE); 
			}
			if(FD_ISSET(commfd, &socket_set)){
				bzero(msg, MAXLINE); 
				read(commfd, msg, MAXLINE); 
				printf("%s\n", msg); 
			}
		}
	}while(msg[0]!='n'); 
}

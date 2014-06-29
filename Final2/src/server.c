#include "five.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERV_COMM_PORT 15926
#define MAXLINE 1024

struct sockaddr_in rcvsock; 
int getlen = sizeof(rcvsock), clientfd[10], client = 0; 
fd_set socket_set; 
char *clientIP[2], *clientName[2]; 
int clientPort[2]; 

int commfd; 

void comm_socket_setup(){
	struct sockaddr_in comm_socket; 
	if((commfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("ERROR!!commfd cannot open!!\n"); 
		Log("Error: commfd cannot open. ", 's'); 
		exit(0); 
	}
	printf("communication fd %d\n", commfd); 
	bzero(&comm_socket, sizeof(comm_socket)); 
	comm_socket.sin_family = AF_INET; 
	comm_socket.sin_addr.s_addr = htonl(INADDR_ANY); 
	comm_socket.sin_port = htons(SERV_COMM_PORT); 
	if(bind(commfd, (struct sockaddr*)&comm_socket, sizeof(comm_socket))<0){
		perror("ERROR!!commfd cannot bind!\n");
		Log("Error: commfd cannot bind. ", 's'); 	
		exit(0); 
	}
	if(listen(commfd, 10)<0){
		perror("ERROR!!commfd cannot listen!\n"); 
		Log("Error: commfd cannot listen. ", 's'); 
		exit(0); 
	}
}

int fd_switch(){
	int clifd, i, j;
	char msg[MAXLINE], snd[MAXLINE], logStr[100]; 

	if(FD_ISSET(commfd, &socket_set)){
		if((clifd = accept(commfd, (struct sockaddr*)&rcvsock, &getlen))<0){
			perror("ERROR!!commfd cannot accept!\n"); 
			Log("Error: commfd cannot accept. ", 's'); 
			return -1; 
		}
		clientIP[client] = inet_ntoa(rcvsock.sin_addr); 
		clientPort[client] = ntohs(rcvsock.sin_port); 
		sprintf(logStr, "new client %s:%d, fd:%d ", clientIP[client], clientPort[client], clifd); 
		printf("%s\n", logStr); 
		Log(logStr, 's'); 
		clientfd[client++] = clifd; 
		return clifd; 
	}
	for(i=0;i<2;i++){
		bzero(msg, MAXLINE); 
		bzero(snd, MAXLINE); 
		if(clientfd[i]>0){
			if(FD_ISSET(clientfd[i], &socket_set)){
				if(read(clientfd[i], msg, MAXLINE)>0){
					printf("recv msg from %s:%d\n> %s\n", clientIP[i], clientPort[i], msg); 
					sprintf(snd, "[%s][%d]:[%s]", clientIP[i], clientPort[i], msg); 
					Log(snd, 'm'); 
					for(j=0;j<2;j++)
						if(clientPort[j]!=0)
							send(clientfd[j], snd, MAXLINE, 0); 
				}
			}
		}
	}
}

int main(int argc, char *argv[]){
	int i=0;
	struct timeval timeout; 
	timeout.tv_sec = 0; 
	timeout.tv_usec = 0; 
	bzero(&clientfd, 2); 
	for(i=0;i<2;i++)
		clientPort[i] = 0; 
	comm_socket_setup(); 
	FD_ZERO(&socket_set); 
	FD_SET(commfd, &socket_set); 
	while(1){
		if(select(10, &socket_set, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)0)>=0){
			fd_switch(); 
			FD_ZERO(&socket_set);  
			FD_SET(commfd, &socket_set); 
			for(i=0;i<2;i++)
				if(clientfd[i]>0)
					FD_SET(clientfd[i], &socket_set); 
		}
	}
}


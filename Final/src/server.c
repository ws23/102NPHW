#include "five.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define MAXLINE 1024
#define SERVPORT 1243

typedef struct _client{
	char IP[16], name[50]; 
	int port, fd; 
} client; 

int main(){
	int i, j, sockfd, getlen, clientNum, clntfd, correct;  
	char msg[MAXLINE], snd[MAXLINE], logStr[50], *tmp; 
	struct timeval timeout; 
	struct sockaddr_in revsock, commsock; 
	fd_set socket_set; 
	client clnt[2]; 
	pack packet; 

	timeout.tv_sec = 0; 
	timeout.tv_usec = 0; 
	getlen = sizeof(revsock); 
	clientNum = 0; 
	bzero(&clnt, sizeof(clnt));
	clnt[0].port = clnt[1].port = 0; 

	// communicate socket setup
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("Socket Error!!\n"); 
		Log("Socket Error. ", 's'); 
		exit(1); 
	}
	printf("Communication fd %d\n", sockfd); 
	bzero(&commsock, sizeof(commsock)); 
	commsock.sin_family = AF_INET; 
	commsock.sin_addr.s_addr = htonl(INADDR_ANY);
	commsock.sin_port = htons(SERVPORT); 

	if(bind(sockfd, (struct sockaddr*)&commsock, sizeof(commsock))<0){
		printf("Socket Bind Error. \n"); 
		Log("Socket Bind Error. ", 's'); 
		exit(1); 
	}

	if(listen(sockfd, 2)<0){
		printf("Socket Listen Error. \n"); 
		Log("Socket Listen Error. ", 's'); 
		exit(1); 
	}

	FD_ZERO(&socket_set); 
	FD_SET(sockfd, &socket_set); 

	while(1){
		if(select(20, &socket_set, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)0)>=0){
			// add new client
			if((clntfd = accept(sockfd, (struct sockaddr*)&revsock, &getlen))<0){
				printf("Socket Accept Error. \n"); 
				Log("Socket Accept Error. ", 's'); 
				exit(1); 

				strcpy(clnt[clientNum].IP, inet_ntoa(revsock.sin_addr)); 
				clnt[clientNum].port = ntohs(revsock.sin_port); 
				sprintf(logStr, "New Client %s:%d, fd: %d . ", clnt[clientNum].IP, clnt[clientNum].port, clntfd); 
				printf("%s\n", logStr); 
				Log(logStr, 's'); 
				Log(logStr, 'c'); 
				Log(logStr, 'm'); 

				clnt[clientNum++].fd = clntfd;
			}
			// Process the packet from client	
			else if(clientNum==2){
				for(i=0;i<2;i++){
					bzero(msg, MAXLINE); 
					bzero(snd, MAXLINE); 
					if(clnt[i].fd>0){
						if(FD_ISSET(clnt[i].fd, &socket_set)){
							if(read(clnt[i].fd, msg, MAXLINE)>0){
								printf("Get Packet from %s:%d\n", clnt[i].IP, clnt[i].port);
								// check packet
								correct = -1; 
								tmp = strtok(msg, " "); 
								if(*tmp=='0'){ // hello (cmd, length, checksum)
									packet.cmd = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									packet.length = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									packet.checksum = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									if(tmp!=NULL || packet.length!=4){
										printf("Packet Error: Packet Length Error. \n"); 
										Log("Packet Error: Packet Length Error.", 's'); 
										continue; 
									}
									if(packet.checksum != checksum(packet)){
										printf("Packet Error: Checksum Different. \n"); 
										Log("Packet Error: Checksum Different. ", 's'); 
										continue; 
									}
									packet.cmd = 0; 
									packet.length = 4+1; 
									packet.data[0] = i; 
									packet.checksum = checksum(packet); 
									correct = 1; 
								}
								else if(*tmp=='1'){ // chess (cmd, length, checksum, x, y)
									packet.cmd = atoi(tmp); 
									tmp = strtok(NULL, " ");
									packet.length = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									packet.checksum = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									packet.data[0] = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									packet.data[1] = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									if(tmp!=NULL || packet.length!=4+2){
										printf("Packet Error: Packet Length Error. \n"); 
										Log("Packet Error: Packet Length Error. ", 's'); 
										continue; 
									}
									if(packet.checksum != checksum(packet)){
										printf("Packet Error: Checksum Different. \n"); 
										Log("Packet Error: Checksu, Different. ", 's'); 
										continue; 
									}
									packet.cmd = 1; 
									packet.length = 4+1+2; 
									packet.data[2] = packet.data[1]; 
									packet.data[1] = packet.data[0]; 
									packet.data[0] = i; 
									correct = 1; 
								}
								else if(*tmp=='2'){ // chat (cmd, length, checksum, message)
									packet.cmd = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									packet.length = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									packet.checksum = atoi(tmp); 
									tmp = strtok(NULL, " "); 
									strcpy(packet.data, tmp); 
									tmp = strtok(NULL, " "); 
									if(tmp!=NULL){
										printf("Packet Error: Packet Length Error. \n"); 
										Log("Packet Error: Packet Length Error. ", 's'); 
										continue; 
									}
									if(packet.checksum != checksum(packet)){
										printf("Packet Error: Checksum Different. \n"); 
										Log("Packet Error: Checksum Different. ", 's'); 
										continue; 
									}
									packet.cmd = 2; 
									packet.length = 4+1+strlen(packet.data); 
									for(j=strlen;j>0;j++)
										packet.data[j] = packet.data[j-1]; 
									packet.data[0] = i; 
									correct = 1; 	
								}
								else{
									printf("Packet Error: Unknown Command Type. \n"); 
									Log("Packet Error: Unknown Command Type. ", 's'); 
								}
								// save & judge
								if(correct==1){
									if(packet.cmd==1){
										chess(packet.data[0], packet.data[1], packet.data[2]); 
										correct = judge(); 
									}
									else if(packet.cmd==2)
										chat(packet.data[0], &packet.data[1]); 
								}
								if(correct!=-1){
									if(packet.cmd==0)
										sprintf(snd, "%u %u %u %d", packet.cmd, packet.length, packet.checksum, packet.data[0]); 
									else if(packet.cmd==1)
										sprintf(snd, "%u %u %u %d %d %d", packet.cmd, packet.length, packet.checksum, packet.data[0], packet.data[1], packet.data[2]); 
									else if(packet.cmd==2)
										sprintf(snd, "%u %u %u %d %s", packet.cmd, packet.length, packet.checksum, packet.data[0], &packet.data[1]); 
									// braodcast
									for(j=0;j<2;j++)
										if(clnt[j].port!=0)
											send(clnt[j].fd, snd, MAXLINE, 0); 
									
								}
								if(correct==1||correct==2){
									packet.cmd = 3; 
									packet.length = 4+1; 
									packet.data[0] = correct; 
									packet.checksum = checksum(packet); 
									sprintf(snd, "%u %u %u %d", packet.cmd, packet.length, packet.checksum, packet.data[0]); 
									for(j=0;j<2;j++)
										if(clnt[j].port!=0)
											send(clnt[j].fd, snd, MAXLINE, 0); 
								}
							}
						}
					}
				}
			}

			FD_ZERO(&socket_set); 
			FD_SET(sockfd, &socket_set); 
			for(i=0;i<2;i++)
				if(clnt[i].fd>0)
					FD_SET(clnt[i].fd, &socket_set);
		}	
	}
}

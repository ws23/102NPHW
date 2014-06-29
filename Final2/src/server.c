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
int getlen = sizeof(rcvsock), clientfd[3], clientPort[3], client = 0; 
fd_set socket_set; 
char *clientIP[3], *clientName[3]; 
int state; // 0: initial, 1-2: who's turn


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
	int clifd, i, j, winner;
	char msg[MAXLINE], snd[MAXLINE], logStr[100], *tmp; 
	pack data; 

	if(FD_ISSET(commfd, &socket_set)){
		if((clifd = accept(commfd, (struct sockaddr*)&rcvsock, &getlen))<0){
			perror("ERROR!!commfd cannot accept!\n"); 
			Log("Error: commfd cannot accept. ", 's'); 
			return -1; 
		}
		clientIP[client] = inet_ntoa(rcvsock.sin_addr); 
		clientPort[client] = ntohs(rcvsock.sin_port); 
		sprintf(logStr, "New client %s:%d, fd:%d ", clientIP[client], clientPort[client], clifd); 
		printf("%s\n", logStr); 
		Log(logStr, 's');
		Log(logStr, 'm'); 
		Log(logStr, 'c'); 
		
		clientfd[client++] = clifd; 
		printf("clientNum = %d\n", client); 	
		return clifd; 
	}
	// Process the packet from client
	for(i=0;i<3;i++){
		bzero(msg, MAXLINE); 
		bzero(snd, MAXLINE);
		bzero(&data, sizeof(data)); 	
		if(clientfd[i]>0){
			if(FD_ISSET(clientfd[i], &socket_set)){
				if(read(clientfd[i], msg, MAXLINE)>0){
					sprintf(logStr, "Get Packet from %s:%d\n> %s\n", clientIP[i], clientPort[i], msg);
					printf("%s\n", logStr); 
					Log(logStr, 's'); 
					
					// check packet & process (save & judge)
					tmp = strtok(msg, "|");
					if(tmp==NULL)
						continue; 
					data.cmd = atoi(tmp); 
					if(data.cmd==1){ // chess (cmd|length|checksum|x|y)
						// read
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.length = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.checksum = atoi(tmp);                                                                            
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.data[0] = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.data[1] = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp!=NULL || data.length!=6)
							continue; 
						if(data.checksum != checksum(data)){
							printf("Packet Error: Checksum Different. \n"); 
							Log("Packet Error: Checksum Different. ", 's'); 
							continue; 
						}  
						if(state!=i+1)
							continue; 

						// process packet & judge
						if(chess(i+1, data.data[0], data.data[1])==0)
							state = i==1?1:2; 	
						winner = 0; 
						winner = judge(); 

						// send preprocess	
						data.cmd = 1u; 
						data.length = 7u; 
						data.data[2] = data.data[1]; 
						data.data[1] = data.data[0];  
						data.data[0] = i+1;
						data.checksum = checksum(data); 

						// send
						sprintf(snd, "%u|%u|%u|%d|%d|%d", data.cmd, data.length, data.checksum, data.data[0], data.data[1], data.data[2]); 
						printf("Send Packet: %s\n", snd); 
						for(j=0;j<2;j++)
							if(clientPort[j]!=0)
								send(clientfd[j], snd, MAXLINE, 0); 

						// send judge 
						bzero(snd, MAXLINE); 
						// send preprocess
						data.cmd = 3u; 
						data.length = 5u; 
						data.data[0] = winner; 
						data.checksum = checksum(data); 

						sprintf(logStr, "%d wins!!", winner); 
						printf("%s\n", logStr); 
						Log(logStr, 'c'); 

						// send
						sprintf(snd, "%u|%u|%u|%d", data.cmd, data.length, data.checksum, data.data[0]);
						printf("Send Packet: %s\n", snd); 
						for(j=0;j<2;j++)
							if(clientPort[j]!=0)
								send(clientfd[j], snd, MAXLINE, 0); 
					}
					else if(data.cmd==2){	// chat (cmd|length|checksum|msg)
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.length = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.checksum = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						strcpy(data.data, tmp); 
						if(data.checksum != checksum(data)){
							printf("Packet Error: Checksum Different. \n"); 
							Log("Packet Error: Checksum Different. ", 's'); 
							continue; 
						}

						// packet process
						chat(i+1, data.data); 

						// send preprocess
						data.cmd = 2u; 
						data.length = 4u + 1u + strlen(data.data); 
						for(j=strlen(data.data); j>0;j--)
							data.data[j] = data.data[j-1]; 
						data.data[0] = i+1; 
						data.checksum = checksum(data); 

						// send
						sprintf(snd, "%u|%u|%u|%d|%s", data.cmd, data.length, data.checksum, data.data[0], &data.data[1]);
						printf("Send Packet: %s\n", snd); 
						for(j=0;j<2;j++)
							if(clientPort[j]!=0)
								send(clientfd[j], snd, MAXLINE, 0); 
					}
					else if(data.cmd==3){ // end (cmd|length|checksum)
						// read
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.length = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.checksum = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp!=NULL || data.length!=4)
							continue; 
						if(data.checksum != checksum(data)){
							printf("Packet Error: Checksum Different. \n"); 
							Log("Packet Error: Checksum Different. ", 's'); 
							continue; 
						}

						winner = judge(); 

						// send preprocess
						data.cmd = 3u; 
						data.length = 5u; 
						data.data[0] = winner; 
						data.checksum = checksum(data); 

						// send
						sprintf(snd, "%u|%u|%u|%d", data.cmd, data.length, data.checksum, data.data[0]);
						printf("Send Packet: %s\n", snd); 
						if(winner==1||winner==2){
							sprintf(logStr, "%d win!", winner); 
							printf("%s\n", logStr); 
							Log(logStr, 'c'); 
							client = 0; 
						}
						for(j=0;j<2;j++)
							if(clientPort[j]!=0)
								send(clientfd[j], snd, MAXLINE, 0); 
					}
					else if(data.cmd==4){ // start (cmd|length|checksum)
						// read
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.length = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp==NULL)
							continue; 
						data.checksum = atoi(tmp); 
						tmp = strtok(NULL, "|"); 
						if(tmp!=NULL || data.length!=4)
							continue; 
						if(data.checksum != checksum(data)){
							printf("Packet Error: Checksum Different. \n"); 
							Log("Packet Error: Checksum Different. ", 's'); 
							continue; 
						}

						// send preprocess & send
						data.cmd = 4u; 
						data.length = 5u; 
						
						if(client==1){
							data.data[0] = -1; 
							data.checksum = checksum(data); 
							sprintf(snd, "%u|%u|%u|%d", data.cmd, data.length, data.checksum, data.data[0]);
							printf("Send Packet: %s\n", snd); 	
							send(clientfd[j], snd, MAXLINE, 0); 
						}
						else if(client==2){
							data.data[0] = 1; 
							data.checksum = checksum(data); 
							sprintf(snd, "%u|%u|%u|%d", data.cmd, data.length, data.checksum, data.data[0]); 
							printf("Send Packet: %s\n", snd); 
							send(clientfd[0], snd, MAXLINE, 0); 

							data.data[0] = 2; 
							data.checksum = checksum(data); 
							sprintf(snd, "%u|%u|%u|%d", data.cmd, data.length, data.checksum, data.data[0]); 
							printf("Send Packet: %s\n", snd); 
							send(clientfd[1], snd, MAXLINE, 0);

							state = 1; 	
						}
						else if(client==3){
							data.data[0] = 3; 
							data.checksum = checksum(data); 
							sprintf(snd, "%u|%u|%u|%d", data.cmd, data.length, data.checksum, data.data[0]); 
							printf("Send Packet: %s\n", snd); 
							send(clientfd[2], snd, MAXLINE, 0); 
							client = 2; 
						}
					}
					else{
						printf("Packet Error: Unknown Command Type. \n"); 
						Log("Packet Error: Unknown Command Type. ", 's'); 
					}
				}
			}
		}
	}
}

int main(int argc, char *argv[]){
	int i=0;
	bzero(&clientfd, 3); 
	for(i=0;i<3;i++)
		clientPort[i] = 0;
	ini(); 	
	state = 0;

	comm_socket_setup(); 

	FD_ZERO(&socket_set); 
	FD_SET(commfd, &socket_set); 
	while(1){
		if(select(20, &socket_set, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)0)>=0){
			fd_switch(); 
			FD_ZERO(&socket_set);  
			FD_SET(commfd, &socket_set); 
			for(i=0;i<3;i++)
				if(clientfd[i]>0)
					FD_SET(clientfd[i], &socket_set); 
		}
	}
}


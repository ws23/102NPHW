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
	char logStr[100]; 
	if((commfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		perror("ERROR!!commfd cannot open!!\n");
		Log("Error. commfd cannot open. ", 's'); 	
		return -1; 
	}
	sprintf(logStr, "communication fd %d", commfd); 
	printf("%s\n", logStr); 
	Log(logStr, 's'); 
	
	bzero(&comm_socket, sizeof(comm_socket)); 
	comm_socket.sin_family = AF_INET; 
	comm_socket.sin_port = htons(servPort); 
	if(inet_pton(AF_INET, servIP, &comm_socket.sin_addr)<=0){
		perror("ERROR!!inet_pton error!!\n"); 
		Log("Error: inet_pton error. ", 's'); 
		return -1; 
	}
	if(connect(commfd, (struct sockaddr*)&comm_socket, sizeof(comm_socket))<0){
		perror("ERROR!!commfd cannot connect!\n"); 
		Log("Error: commfd cannot connect. ", 's'); 
		return -1; 
	}
	if(getsockname(commfd, (struct sockaddr*)&comm_socket, &len)<0){
		perror("ERROR!!commfd cannot use getsockbyname\n"); 
		Log("Error: commfd cannot use getsockbyname", 's'); 
		//return -1; 
	}
	sprintf(logStr, "Local IP is %s:%d", inet_ntoa(comm_socket.sin_addr), ntohs(comm_socket.sin_port)); 
	printf("%s\n", logStr); 
	Log(logStr, 's'); 
	Log(logStr, 'm'); 
	Log(logStr, 'c');
	return 0; 
}

int main(int argc, char *argv[]){
	char msg[MAXLINE], snd[MAXLINE], *IP, logMsg[100], *tmp; 
	int ID, state; 
	struct hostent *host; 
	fd_set socket_set; 
	pack data; 
	if(argc!=2){
		printf("Usage: %s <IPaddress>\n", argv[0]); 
		exit(0); 
	}
	state = 0; // 0: initial, 1: wait, 2: play_chess, 3:  play_no_chess
	host = gethostbyname(argv[1]);
	ini(); 
	sprintf(logMsg, "Host Name: %s, Host IP: %s", host->h_name, inet_ntoa(*(struct in_addr*)host->h_addr)); 
	printf("%s\n", logMsg); 
	Log(logMsg, 's'); 

	if(comm_socket_setup(inet_ntoa(*(struct in_addr*)host->h_addr), SERV_COMM_PORT)<0)
		exit(0); 

	// send start game request
	data.cmd = 4u; 
	data.length = 4u; 
	data.checksum = checksum(data);
	sprintf(snd, "%u|%u|%u", data.cmd, data.length, data.checksum); 
	write(commfd, snd, MAXLINE);

	while(1){
		bzero(msg, MAXLINE);
		FD_ZERO(&socket_set); 
		FD_SET(commfd, &socket_set); 
		FD_SET(0, &socket_set);
		if(select(20, &socket_set, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)0)>=0){
			// send
			if(FD_ISSET(0, &socket_set)){
				if(state==2||state==3){
					gets(msg); 

					// preprocess
					tmp = strtok(msg, " "); 
					if(*tmp=='c'||*tmp=='C'){
						data.cmd = 1u; 
						data.length = 6u;
						tmp = strtok(NULL, " "); 
						if(tmp==NULL)	
							continue; 
						if(*tmp<'A' || *tmp>'Z')
							continue; 
						data.data[0] = *tmp-'A'+1; 
						tmp = strtok(NULL, " "); 
						if(tmp==NULL)
							continue; 
						data.data[1] = atoi(tmp); 
						tmp = strtok(NULL, " "); 
						if(tmp!=NULL)
							continue; 
						data.checksum = checksum(data);

						sprintf(snd, "%u|%u|%u|%d|%d", data.cmd, data.length, data.checksum, data.data[0], data.data[1]); 
						write(commfd, snd, MAXLINE); 
					}
					else if(*tmp=='m'||*tmp=='M'){
						data.cmd = 2u; 
						tmp = strtok(NULL, " "); 
						if(tmp==NULL)
							continue; 
						data.length = strlen(tmp); 
						strcpy(data.data, tmp); 
						data.checksum = checksum(data); 
						tmp = strtok(NULL, " "); 
						if(tmp!=NULL){
							printf("Cannot contain the space character in the message. \n"); 
							continue; 
						}

						sprintf(snd, "%u|%u|%u|%s", data.cmd, data.length, data.checksum, data.data); 
						write(commfd, snd, MAXLINE);
					}	
				}
			}
			// read
			if(FD_ISSET(commfd, &socket_set)){
				bzero(msg, MAXLINE); 
				read(commfd, msg, MAXLINE); 
				sprintf(logMsg, "Get Packet from Server\n> %s", msg); 
				Log(logMsg, 's'); 

				tmp = strtok(msg, "|"); 
				data.cmd = atoi(tmp); 
				if(data.cmd==1){ // chess (cmd|length|checksum|clientID|x|y)
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
					if(tmp==NULL)
						continue;
					data.data[2] = atoi(tmp); 
					tmp = strtok(NULL, "|"); 
					if(tmp!=NULL || data.length!=7)
						continue;
					if(data.checksum != checksum(data))
						continue;

					// process
					if(chess(data.data[0], data.data[1], data.data[2])==0){
						if(data.data[0]!=clientNum)
							state = 2;
						else
							state = 3; 	
					}
					else if(state==2&&clientNum==data.data[0])
						printf("You chess on the site that has already had been chessed. \n");

					// judge
					bzero(msg, MAXLINE);
					data.cmd = 3u; 
					data.length = 4u; 
					data.checksum = checksum(data); 
					sprintf(snd, "%u|%u|%u", data.cmd, data.length, data.checksum); 
					printf("Send Packet: %s\n", snd); 
					send(commfd, snd, MAXLINE, 0); 
				}
				else if(data.cmd==2){ // chat (cmd|length|checksum|clientID|message)
					tmp = strtok(NULL, "|"); 
					if(tmp==NULL)
						continue;
					data.length = atoi(tmp); 
					tmp = strtok(NULL, "|"); 
					if(tmp==NULL)
						continue;
					data.checksum = checksum(data); 
					tmp = strtok(NULL, "|"); 
					if(tmp==NULL)
						continue;
					data.data[0] = atoi(tmp); 
					tmp = strtok(NULL, "|"); 
					if(tmp==NULL)
						continue;
					strcpy(&data.data[1], tmp); 
					if(data.checksum != checksum(data))
						continue;

					// process
					chat(data.data[0], &data.data[1]);
				}
				else if(data.cmd==3){ // end (cmd|length|checksum|winner clientID)
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
					if(tmp!=NULL || data.length!=5u)
						continue;
					if(checksum(data) != data.checksum)
						continue;
					if(data.data[0]==-1){
						printf("There are 2 players on the server. Please connect later again. \n"); 
						break;
					}
					if(data.data[0]==0)
						continue; 
					sprintf(logMsg, "%s", data.data[0]==clientNum?"You Win!!":"You Lose!!"); 
					printf("%s\n", logMsg); 
					Log(logMsg, 'c'); 

					// process
					break; 
				}
				else if(data.cmd==4){ // start game (cmd|length|checksum|clientID(1, 2, -1:reject))
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
					if(tmp!=NULL || data.length!=5)
						continue; 
					if(checksum(data) != data.checksum)
						continue; 

					// process
					if(data.data[0]==-1)
						printf("Waiting for other player. \n"); 
					else if(data.data[0]==1){
						printf("Game Start. You are first. \n"); 
						Log("Game Start. You are first. ", 'c'); 
						state = 2; 
						clientNum = 1; 
					}
					else if(data.data[0]==2){
						printf("Game Start. You are second. \n"); 
						Log("Game Start. You are second. ", 'c'); 
						state = 3; 
						clientNum = 2; 
					}
					else if(data.data[0]==3){
						printf("There are 2 clients connect to server. Please connect later. \n"); 
						break;
					}
				}
				else{
					printf("Packet Error: Command Type Error. \n"); 
					Log("Packet Error: Command type Error. ", 's'); 
				}
				if(state==2||state==3){
					show(); 
					if(state==2)
						printf("It's your turn.\nc <x> <y> to chess, "); 
					printf("m <message> to chat. (Do not send more than 50 characters.)\n"); 
					printf("> ");
				} 
			}
		}
	} 
}

#include "five.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#define MAXLINE 1024
#define SERVPORT 15692

int main(int argc, char *argv[]){
	int clientNum, sockfd, rel; 
	char msg[50], buff[MAXLINE], logStr[100], *tmp, in[MAXLINE]; 
	struct sockaddr_in servaddr; 
	struct hostent *host; 
	pack data, rev; 
	fd_set socket_set; 

	if(argc!=2){
		printf("usage: %s <IP Address>\n", argv[0]); 
		exit(0); 
	}

	ini(); 
	host = gethostbyname(argv[1]); 
	sprintf(logStr, "hostname: %s, hostIP: %s", host->h_name, inet_ntoa(*(struct in_addr*)host->h_addr)); 
	printf("%s\n", logStr); 
	Log(logStr, 's'); 
	Log(logStr, 'c'); 
	Log(logStr, 'm'); 

	// communicate socket setup
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("socket error. \n"); 
		Log("socket error. ", 's'); 
		exit(1); 
	}
	printf("Communuicate fd %d\n", sockfd); 	
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(SERVPORT); 
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0){
		sprintf(logStr, "inet_pton error for %s. ", argv[1]); 
		printf("%s\n", logStr); 
		Log(logStr, 's');
		exit(1); 
	}
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
		sprintf(logStr, "Connect to %s Error. ", argv[1]); 
		printf("%s\n", logStr); 
		Log(logStr, 's'); 
		exit(1); 
	}
	sprintf(logStr, "Local IP is %s:%d", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port)); 
	printf("%s\n", logStr); 
	Log(logStr, 's'); 
	Log(logStr, 'c'); 
	Log(logStr, 'm'); 

	show(); 
	printf("A"); 
	while(1){
		printf("B"); 
		bzero(buff, MAXLINE); 
		printf("C"); 
		FD_ZERO(&socket_set); 
		printf("D"); 
		FD_SET(sockfd, &socket_set); 
		printf("E"); 
		FD_SET(0, &socket_set);
		printf("F"); 
			rel = select(6, &socket_set, (fd_set*)NULL, (fd_set*)NULL, (struct timeval*)0); 
			printf("%d", rel);
		if(rel>=0){
			printf("G"); 
			if(FD_ISSET(0, &socket_set)){
				printf("H"); 
				// hello 
				printf("I"); 
				data.cmd = 0u; 
				data.length = 4; 
				data.checksum = checksum(data); 
				printf("J"); 
				sprintf(buff, "%u %u %u", data.cmd, data.length, data.checksum); 
				printf("K"); 
				write(sockfd, buff, MAXLINE); 
				printf("下棋：c <X座標> <Y座標>（ex. c A 10）\n聊天：m <訊息>（ex. m 安安你好！）※ 訊息勿超過40字元\n"); 
				gets(in);
				printf("[%s]\n", in);
				tmp = strtok(in, " "); 
				// chess
				if(*tmp=='c'||*tmp=='C'){
					data.cmd = 1u; 
					tmp = strtok(NULL, " "); 
					data.length = 4+2;
					if(tmp==NULL){
						printf("Command Error. \n"); 
						continue; 
					}	
					data.data[0] = *tmp-'A'+1;
					tmp = strtok(NULL, " "); 	
					if(tmp==NULL){
						printf("Command Error. \n"); 
						continue; 
					}
					data.data[1] = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					if(tmp!=NULL)
						continue; 
					data.checksum = checksum(data); 
					sprintf(buff, "%u %u %u %d %d", data.cmd, data.length, data.checksum, data.data[0], data.data[1]); 
					write(sockfd, buff, MAXLINE);
					Log("Send Chess. ", 's'); 	
				}
				// chat
				else if(*tmp=='m'||*tmp=='M'){
					data.cmd = 2u; 
					tmp = strtok(NULL, " "); 
					if(strlen(tmp)>45){
						printf("Message is too long. \n"); 
						continue; 
					}
					else if(tmp==NULL){
						printf("Command Error. \n"); 
						continue; 
					}
					data.length = 4+strlen(msg); 
					strcpy(data.data, tmp); 
					data.checksum = checksum(data); 
					sprintf(buff, "%u %u %u %s", data.cmd, data.length, data.checksum, data.data); 
					write(sockfd, buff, MAXLINE); 
					Log("Send Caht. ", 's'); 
				}
				else
					printf("Command Error. \n"); 	
			}
			if(FD_ISSET(sockfd, &socket_set)){
				printf("Get Packet. \n"); 
				bzero(buff, MAXLINE); 
				read(sockfd, buff, MAXLINE); 
				tmp = strtok(buff, " "); 
				// hello ACK (cmd length checksum clientNum)
				if(*tmp=='0'){
					rev.cmd = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.length = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.checksum = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.data[0] = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					if(tmp!=NULL || rev.length!=4+1){
						printf("Packet Error: Packet Length Error. \n"); 
						Log("Packet Error: Packet Length Error. \n", 's'); 
						continue; 
					}
					if(rev.checksum != checksum(rev)){
						printf("Packet Error: Checksum Different. \n"); 
						Log("Packet Error: Checksum Different. ", 's'); 
						continue; 
					}
					clientNum = rev.data[0]; 
				}
				// chess (cmd length checksum clientNum x y)
				else if(*tmp=='1'){
					rev.cmd = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.length = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.checksum = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.data[0] = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.data[1] = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.data[2] = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					if(tmp!=NULL || rev.length!=4+3){
						printf("Packet Error: Packet Length Error. \n"); 
						Log("Packet Error: packet length Error. ", 's');
						continue; 
					}
					if(rev.checksum != checksum(rev)){
						printf("Packet Error: Checksum Different. \n"); 
						Log("Packet Error: Checksum Different. ", 's'); 
						continue; 
					}
					if(rev.data[0]>2 || rev.data[0]<1){
						printf("Invalid Player ID. \n"); 
						Log("Invalid Player ID. ", 's'); 
						continue; 
					}
					chess(rev.data[0], rev.data[1], rev.data[2]); 
				}
				// chat (cmd length checksum clientNum message)
				else if(*tmp=='2'){
					rev.cmd = atoi(tmp); 
					tmp = strtok(NULL, " ");
					rev.length = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.checksum = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.data[0] = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					strcpy(&rev.data[1], tmp);
					tmp = strtok(NULL, " "); 
					if(tmp!=NULL){
						printf("Packet Error: Packet Length Error. \n"); 
						Log("Packet Error: Packet Length Error. ", 's'); 
						continue;
					}
					if(rev.checksum != checksum(rev)){
						printf("Packet Error: Checksum Different. \n"); 
						Log("Packet Error: Checksum Different. ", 's'); 
						continue; 
					}	
					if(rev.data[0]>2 || rev.data[0]<2){
						printf("Invalid Player ID. \n"); 
						Log("Invalid Player ID. ", 's'); 
						continue; 
					}
					chat(rev.data[0], &rev.data[1]); 
				}
				// end (cmd length checksum clientNum(winner))
				else if(*tmp=='3'){
					rev.cmd = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.length = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.checksum = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					rev.data[0] = atoi(tmp); 
					tmp = strtok(NULL, " "); 
					if(tmp!=NULL || rev.length!=4+1){
						printf("Packet Error: Packet Length Error. \n"); 
						Log("Packet Error: Packet Length Error. ", 's'); 
						continue; 
					}
					if(rev.checksum != checksum(rev)){
						printf("Packet Error: Checksum Different. \n"); 
						Log("Packet Error: Checksum Different. ", 's'); 
						continue; 
					}
					if(rev.data[0]>2 || rev.data[0]<1){
						printf("Invalid Player ID. \n"); 
						Log("Invalid Player ID. ", 's'); 
						continue; 
					}
					break;
				}
				else{
					printf("Packet Error: Unknown Command Type. \n"); 
					Log("Packet Error: Unknown Command Type. ", 's'); 
				}
				show(); 
			}
		}
	}
	close(sockfd); 
	exit(0); 
}

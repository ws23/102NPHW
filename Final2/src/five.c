#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "five.h"
#define MAX 20
int8_t disk[MAX][MAX]; 
char msg[1000][50]; 
int8_t head, tail; 

uint16_t checksum(pack data){
	uint8_t hex[4]; 
	int8_t i, length; 
	uint16_t check; 
	//add groups
	hex[0] = data.cmd>>4; 
	hex[1] = (data.cmd<<4)>>4; 
	hex[2] = data.length>>4; 
	hex[3] = (data.length<<4)>>4; 
	length = data.length; 
	for(i=0;i<length-4;i+=2){
		hex[0] += data.data[i]>>4; 
		hex[1] += (data.data[i]<<4)>>4; 
		hex[2] += data.data[i+1]>>4; 
		hex[3] += (data.data[i+1]<<4)>>4; 
		if(hex[3]>15u){
			hex[3] -= 16u; 
			hex[2] += 1u; 
		}
		if(hex[2]>15u){
			hex[2] -= 16u; 
			hex[1] += 1u; 
		}
		if(hex[1]>15u){
			hex[1] -= 16u; 
			hex[0] += 1u; 
		}
		if(hex[0]>15u){
			hex[0] -= 16u; 
			hex[3] += 1u; 
		}
	}

	// calculate checksum
	check = hex[0]<<12 + hex[1]<<8 + hex[2]<<4 + hex[3]; 
	check = ~check;

	return check; 
}

void ini(){
	memset(disk, 0, sizeof(disk));
	memset(msg, 0, sizeof(msg));	
	tail = head = clientNum = 0; 	
}

int chess(int p, int x, int y){
	char logMsg[50]; 
	if(clientNum!=0)
		sprintf(logMsg, "%s: %c%d\n", p==clientNum?"我":"敵", x+'A'-1, y);
	else
		sprintf(logMsg, "%d: %c%d\n", p, x+'A'-1, y); 
	if(x>MAX || y>MAX || x<0 || y<0)
		return -1; 
	else if(disk[MAX-y][x-1]==0){
		disk[MAX-y][x-1] = p;
		Log(logMsg, 'c'); 
	}
	else
		return -1; 
	return 0; 
}

void chat(int p, char *s){
	char tmp[50];
	if(clientNum!=0)
		sprintf(tmp, "%s: %s", p==clientNum?"我":"敵", s);
	else
		sprintf(tmp, "%d: %s", p, s);
	strcpy(msg[tail], tmp);
	Log(strcat(tmp, "\n"), 'm'); 
	tail++;
	if(head+MAX<tail)
		head++;
}

void show(){
	int i, j;
	printf("   ");
	for(i=1;i<=MAX;i++)
		printf("%c ", 'A'+i-1);
	printf("\n");
	for(i=0;i<MAX;i++){
		printf("%2d ", MAX-i);
		for(j=0;j<MAX;j++){
			if(disk[i][j]==0)
				printf("＋"); 
			else if(disk[i][j]==1)
				printf("☻ "); 
			else if(disk[i][j]==2)
				printf("☺ "); 
		}
		printf(" %s\n", msg[head+i]);
	}
}

void Log(char *s, char type){
	FILE *fout; 
	char strTime[50]; 
	time_t now; 
	time(&now); 
	
	now -= 16*60*60; 
	sprintf(strTime, "[%s", asctime(gmtime(&now))); 
	strtok(strTime, "\n");	

	if(type=='c')
		fout = fopen("log/Chess.log", "a");
	else if(type=='m')
		fout = fopen("log/Chat.log", "a"); 
	else if(type=='s')
		fout = fopen("log/Sys.log", "a"); 
	else{
		printf("Log Type Error. \n"); 
		fout = fopen("log/Sys.log", "a"); 
		fprintf(fout, "Log Type Error. \n"); 
		return; 
	}
	fputs(strcat(strTime, "] "), fout); 
	fputs(s, fout); 
	fputs("\n", fout); 
	fclose(fout); 
}

int Max(int a, int b){
	return a>b?a:b; 
}

int judge(){
	int i, j, k; 
	for(i=0;i<MAX;i++){
		for(j=0;j<MAX;j++){
			if(disk[i][j]==0)
				continue; 
			// left
			for(k=1;k<5;k++){
				if(j-k<0)
					break;
				if(disk[i][j]!=disk[i][j-k])
					break;
				if(k==4)
					return disk[i][j]; 
			}
			// up
			for(k=1;k<5;k++){
				if(i-k<0)
					break;
				if(disk[i][j]!=disk[i-k][j])
					break;
				if(k==4)
					return disk[i][j]; 
			}
			// left-up
			for(k=1;k<5;k++){
				if(i-k<0)
					break;
				if(j-k<0)
					break;
				if(disk[i][j]!=disk[i-k][j-k])
					break;
				if(k==4)
					return disk[i][j]; 
			}
			// right-up
			for(k=1;k<5;k++){
				if(j+k>=MAX)
					break;
				if(i-k<0)
					break;
				if(disk[i][j]!=disk[i-k][j+k])
					break;
				if(k==4)
					return disk[i][j]; 
			}
		}
	}
	return 0; 
}

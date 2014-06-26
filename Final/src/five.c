#include <stdio.h>
#include <string.h>
#include "five.h"
#define MAX 20
int8_t disk[MAX][MAX]; 
char msg[1000][50]; 
int8_t head, tail; 

struct packet{
	uint8_t cmd;
	uint8_t length; 
	uint16_t checksum; 
	int8_t data[254]; 
}; 

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
	sprintf(logMsg, "%s: %c%d\n", p==clientNum?"我":"敵", x+'A'-1, y); 
	if(disk[MAX-y][x-1]==0){
		disk[MAX-y][x-1] = p;
		Log(logMsg, 'c'); 
	}
	else
		return -1; 
	return 0; 
}

void chat(int p, char *s){
	char tmp[50];
	sprintf(tmp, "%s: %s", p==clientNum?"我":"敵", s);
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
	fputs(s, fout); 
	fclose(fout); 
}

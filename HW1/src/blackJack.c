#include "blackJack.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int iniCard[52];
int pCard[PLAYER][5];  
int pBet[PLAYER];

void showHelp(){
	FILE *fin;
	char str;
	fin = fopen("Rules", "r");
	do{
		str = fgetc(fin);
		putc(str);
	}while(str != EOF);
	fclose(fin);
}

void ini(){
	int i, j;
	for(i=0;i<52;i++)
			iniCard[i] = i+1;
	for(i=0;i<PLAYER;i++){
		for(j=0;j<5;j++)
			pCard[i][j] = 0;
		pBet = BET;
	}
}

void swap(int *a, int*b){
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void shuffle(){
	int i, times;
	srand(time(NULL));
	times = rand()%2000000000+147483647;
	for(i=0;i<times;i++)	
		swap(iniCard[rand()%52], iniCard[rand()%52]);
}

int getPoint(int index){

}

void showCard(int index){
	if()
}

void showCards(){
	int i, j;
	for(i=0;i<PLAYER;i++){
		for(j=0;j<5;j++){
			if(pCard[i][j]==0)
				continue;
			printf("")
		}
	}

}

void deal(){
	
}

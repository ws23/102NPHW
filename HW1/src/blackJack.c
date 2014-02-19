#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "blackJack.h"

// Initial Constant

const int BET = 10;
const int SELF = 0;
const char pName[PLAYER][100] = {"Player", "Computer"}; 

// ==============================================

int iniCard[52];
int pCard[PLAYER][6];  
int pBet[PLAYER], pbBet[PLAYER];
int first = 1;
int top = 52;
int showFinal = 0;

void showHelp(){
	FILE *fin;
	char str;
	fin = fopen("Rules", "r");
	do{
		str = fgetc(fin);
		putchar(str);
	}while(str != EOF);
	fclose(fin);
}

void iniAgain(){
	int i, j;
	for(i=0;i<52;i++)
		iniCard[i] = i+1;
	for(i=0;i<PLAYER;i++){
		for(j=0;j<6;j++)		
			pCard[i][j] = 0;
		pbBet[i] = 1;
		pBet[i] -= pbBet[i];
		top = 52;
	}
	showFinal = 0;
}

void ini(){
	int i, j;
	for(i=0;i<52;i++)
			iniCard[i] = i+1;
	for(i=0;i<PLAYER;i++){
		for(j=0;j<6;j++)
			pCard[i][j] = 0;
		pbBet[i] = 1;
		pBet[i] = BET-1;
		top = 52;
	}
	first = 0;
	showFinal = 0;
}

void swap(int *a, int*b){
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void shuffle(){
	int i, times;
	if(first)
		ini();
	else
		iniAgain();
	srand(time(NULL));
	times = rand()%50000+100;
	for(i=0;i<times;i++)	
		swap(&iniCard[rand()%52], &iniCard[rand()%52]);
}

int getPoint(int index){
	int tmp;
	switch(index%13){
		case 11:
			;
		case 12:
			;
		case 0:
			tmp = 10;
			break;
		case 1:
			tmp = 1;
			break;
		default:
			tmp = index%13;
	}
	return tmp;
}

int countPoints(int pIndex){
	int i, total = 0;
	for(i=0;i<5;i++){
		if(pCard[pIndex][i]==0)
			continue;
		total += getPoint(pCard[pIndex][i]);
		if(pCard[pIndex][i]%13==1)
			if(total+10<=21)
				total+=10;
	}
	return total;
}

void showCard(int index){
	switch((int)(index-1)/13){
		case 0:
			printf("♣ ");
			break;
		case 1:
			printf("♦ ");
			break;
		case 2:
			printf("♥ ");
			break;
		case 3:
			printf("♠ ");
			break;
	}
	switch(index%13){
		case 11:
			printf("J");
			break;
		case 12:
			printf("Q");
			break;
		case 0:
			printf("K");
			break;
		case 1:
			printf("A");
			break;
		default:
			printf("%d", index%13);
	}
}

void showCards(){
	int i, j, tmp;
	printf("\n");
	for(i=0;i<PLAYER;i++){
		printf("%s [have %d bet(s), bet %d bet(s)] :\n\t", pName[i], pBet[i], pbBet[i]);
		for(j=0;j<5;j++){
			if(pCard[i][j]==0)
				continue;
			if(j==0 && i!=SELF && pCard[i][5]!=-1 && showFinal==0){
				printf("???  ");
				continue;
			}
			showCard(pCard[i][j]);	
			printf("  ", pCard[i][j]);
		}
/*		tmp = countPoints(i);
		if(i!=SELF && pCard[i][5]!=-1)
			tmp -= getPoint(pCard[i][0]);
		printf("\tTotal Points: %d\n\n", tmp);
 */
		if(i==SELF || showFinal!=0)
			printf("\tTotal Points: %d", countPoints(i));
		printf("\n\n");
	}
}

int getTopCard(){
	top--;
	return iniCard[top];
}

void deal(){
	int i, j;
	shuffle();
	for(i=0;i<2;i++)
		for(j=0;j<PLAYER;j++)
			pCard[j][i] = getTopCard();	
	showCards();
}

void processBet(int pIndex, int other){
	pBet[pIndex] += pbBet[pIndex]*other;	
}

void busting(int pIndex){
	pCard[pIndex][5] = -1;
}

int judgeIni(){
	int i, tmp, numWin = 0;
	// 判斷是否有人初始即獲得21點
	for(i=0;i<PLAYER;i++){
		if(countPoints(i)==21 && pCard[i][3]==0){
			numWin++;
			tmp = i;
		}
	}
	if(numWin==1){
		processBet(tmp, 1);
		showFinal = 1;
		showCards();
		printf("%s win this round!!\n", pName[tmp]);
		return tmp;
	}
	return -1;
}

int judge(){
	int i, j, tmp, max, numWin = 0;
	showFinal = 1;
	showCards();
	// 判斷是否僅有一人獲得5張牌且未爆
	for(i=0;i<PLAYER;i++){
		if(pCard[i][5]!=-1 && pCard[i][4]!=0){
			numWin++;
			tmp = i;
		}
	}	
	if(numWin==1){
		processBet(tmp, 2);
		printf("%s win this round!!\n", pName[tmp]);
		return tmp;
	}
	// 判斷誰擁有最大點數
	max = 0;
	for(i=0;i<PLAYER;i++){
		if(max<countPoints(i) && countPoints(i)<=21)
			max = countPoints(i);
	}
	for(i=0;i<PLAYER;i++){
		if(max==countPoints(i)){
			numWin++;
			tmp = i;
		}
	}
	if(numWin==1){
		processBet(tmp, 1);
		printf("%s win this round!!\n", pName[tmp]);
		return tmp;
	}
	printf("No one win this round!!\n");
	return -1;
}

int judgeWin(){
	int i, tmp, lose = 0;
	for(i=0;i<PLAYER;i++){
		if(pBet[i]<=0){
			lose++;
			tmp = i;
		}
	}
	tmp = (tmp==1?0:1);
	if(lose==1){
		printf("%s win this game!!\n", pName[tmp]);
		return tmp;
	}
	return -1;
}

char compOpt(int pIndex){
	int i, cards;
	char option;
	for(i=0;i<5;i++){
		if(pCard[pIndex][i]==0){
			cards = i;
			break;
		}
	}
	if(cards>=5)
		option = '2';
	else{
		option = rand()%3+'1';
		while((countPoints(pIndex)<17 && option=='2') || (pbBet[pIndex]*2>pBet[pIndex] && option=='3'))
			option = rand()%3+'1';
	}
	return option;
}

int selectOpt(int pIndex){
	int i, cards;
	char option;

	if(pIndex!=SELF)
		option = compOpt(pIndex);
	else{
		while(1){
			printf("What do you want to do? (");
			for(i=0;i<5;i++){
				if(pCard[pIndex][i]==0){
					cards = i;
					break;
				}
			}   
			if(cards>=5)
				printf(" 2. Stand");
			else{
				printf(" 1. Hit");
				if(countPoints(pIndex)>=17)
					printf(" 2. Stand");
				if(pbBet[pIndex]*2<=pBet[pIndex])
					printf(" 3. Double");
			}
			printf(" )\n> ");
			option = getchar();
			getchar();
			if(cards>=5 && option!='2')
				continue;
			if(pbBet[pIndex]*2>pBet[pIndex] && option=='3')
				continue;
			if(countPoints(pIndex)<17 && option=='2')
				continue;
			break;
			printf("Input Error!! \n");
		}
	}
	if(option=='1'){
		pCard[pIndex][cards] = getTopCard();
		if(pIndex==SELF){
			printf("You got ");
			showCard(pCard[pIndex][cards]);
			printf("!!\n");
		}
		else{
			printf("%s got ", pName[pIndex]);
			showCard(pCard[pIndex][cards]);
			printf("!!\n");
		}
		if(countPoints(pIndex)>21){
			busting(pIndex);
			if(pIndex==SELF)
				printf("You are busting!!\n");
			else
				printf("%s is busting!!\n", pName[pIndex]);
			showCards();
			return -1;
		}
		showCards();
		cards++;
	}
	else if(option=='2'){
		if(pIndex==SELF)
			printf("You Stand!!\n");
		else
			printf("%s Stand!!\n", pName[pIndex]);
		return -1;
	}
	else if(option=='3'){
		pBet[pIndex] -= pbBet[pIndex];
		pbBet[pIndex] *= 2;	
		if(pIndex==SELF)
			printf("You \"Double\"!!\n");
		else
			printf("%s \"Double\"!!\n", pName[pIndex]);
	}
}

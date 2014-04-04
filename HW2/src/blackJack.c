#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "blackJack.h"

/* declared variable */
pokerCard iniCard; 

/* dealer */
void newGame(){
	int i;
	iniCard.numCard = 52; 
	for(i=0;i<52;i++)
		iniCard.cards[i] = i;	
	srand(time(NULL));
}

void iniGame(){
	iniCard.numCard = 52;
}

void swap(int *a, int *b){
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void shuffle(){
	int i, a, b, n, tmp;
	n = 1000 + rand()%50000;
	for(i=0;i<n;i++){
		a = rand()%52; 
		b = rand()%52;
		swap(&(iniCard.cards[a]), &(iniCard.cards[b]));
	}
}

int dealCard(){
	iniCard.numCard--;
	return iniCard.cards[iniCard.numCard]; 
}

int doDouble(player *p){
	if(p->coins < p->bets){
		printf("%s doesn't have enough coins to do \"Double\"!!\n", p->name);
		return -1;
	}
	p->coins -= p->bets; 
	p->bets *= 2;
	printf("%s has $%d, and bet $%d\n", p->name, p->coins, p->bets);
	return 0;
}

int doStand(player *p){
	if(p->points < 17){
		printf("%s doesn't have enough points to do \"Stand\"!!\n", p->name);
		return -1;
	}
	return 0;
}

player* judgeWinner(player *p1, player *p2){
	player *tmp = NULL;

	/* Judge have 5 hand cards */
	if(p1->numCard==5 && p1->points<=21){
		if(p2->numCard==5 && p2->points<=21){
			if(p1->points>p2->points)
				tmp = p1;
			else if(p2->points>p1->points)
				tmp = p2;
		}
		else
			tmp = p1;
	}
	else if(p2->numCard==5 && p2->points<=21)
		tmp = p2;
	if(tmp!=NULL){
		tmp->coins += tmp->bets*2;
		p1->bets = 0;
		p2->bets = 0;
		return tmp;
	}
	/* Judge Point */
	if(p1->points<=21){
		if(p2->points<=21){
			if(p1->points>p2->points)
				tmp = p1;
			else if(p1->points<p2->points)
				tmp = p2;
		}
		else
			tmp = p1;
	}
	else if(p2->points<=21)
		tmp = p2;
	if(tmp!=NULL)
		tmp->coins += tmp->bets;
	p1->bets = 0;
	p2->bets = 0;
	return tmp;
}
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

/* player */
void newPlayer(player *p, char *Name, int Coin){
	strcpy(p->name, Name);
	p->coins = Coin; 	
}

void iniPlayer(player *p, int Bet){
	int i;
	p->bets = Bet; 
	p->coins -= Bet;
	p->numCard = 0;
	for(i=0;i<5;i++)
		p->cards[i] = -1;
	p->points = 0;
}

int autoBet(player *p){
	int tmp;
	if(p->coins<0){
		printf("%s doesn't have enough money to play. \n", p->name);
		return -1;
	}
	tmp = rand()%(p->coins)+1;
	return tmp;
}

int againGame(){
	int choice; 
	do{
		printf("Do you want to play again? [1. Yes/ 2. No]\n> "); 
		scanf("%1d", &choice);
		if(choice==1)
			return 1;
		else if(choice==2)
			return 0;
		printf("Input Error. Choose again. \n");
	} while(1);
	return -1;
}

int getCard(player *p, int c){
	int Ace, tmp, i;
	Ace = tmp = 0;
	p->cards[p->numCard] = c;
	p->numCard++;
	for(i=0;i<p->numCard;i++){
		switch(p->cards[i]%13){
			case 9:
			case 10:
			case 11:
			case 12:
				tmp += 10;
				break;
			case 0:
				tmp += 11;
				Ace++;
				break;
			default:
				tmp += p->cards[i]%13 + 1;
		}
	}
	while(Ace>0 && tmp >21){
		Ace--;
		tmp -= 10;
	}
	p->points = tmp;
	if(p->points>21)
		return -1;
	return 0;
}

int doSelect(player *p){
	int choice; 
	do{
		printf("What do you want to do? [ 1. HIT / 2. STAND / 3. DOUBLE ]\n> ");
		scanf("%1d", &choice);
		if(choice>=1 && choice <=3)
			return choice;
		printf("Input Error. Choose again. \n");
	}while(1);
	return -1;
}

int autoSelect(player *p){
	int choice;
	choice = rand()%3 + 1;
	return choice; 
}

void showCard(player *p, int mode){
	int i;
	if(mode==1)
		printf("%s's cards\t[%d Points]: \n", p->name, p->points);
	else if(mode==0)
		printf("%s's cards: \n", p->name);
	else
		return;
	if(mode==0){
		printf("%7s ?\t", "??");
		i = 1;
	}	
	else if(mode==1)
		i = 0;
	for(;i<p->numCard;i++){
		switch(p->cards[i]/13){
			case 0: 
				printf("%7s ", "Club");
				break;
			case 1:
				printf("%7s ", "Diamond");
				break;
			case 2:
				printf("%7s ", "Heart");
				break;
			case 3:
				printf("%7s ", "Spade");
				break;
			default:
				printf("Suit error!!");
		}
		switch(p->cards[i]%13){
			case 12:
				printf("K");
				break;
			case 11:
				printf("Q");
				break;
			case 10:
				printf("J");
				break;
			case 0:
				printf("A");
				break;
			default:
				printf("%d", p->cards[i]%13+1);
		}
		printf("\t");
	}
	printf("\n");
}

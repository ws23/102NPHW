#include <stdio.h>
#include <stdlib.h>
#include "blackJack.h"

int main(int argc, char *argv[]){
	int i, tmp;
	player *p, *c, *winner;
	p = (player*)malloc(sizeof(player));
	c = (player*)malloc(sizeof(player));

	newPlayer(p, "Player", 10);
	newPlayer(c, "Computer", 10);
	newGame();
	while(1){
		/* Bet */
		printf("How much do %s want to bet? \n> ", p->name);
		scanf("%d", &i);
		iniPlayer(p, i);
		i = autoBet(c);
		if(i==-1)
			break;
		iniPlayer(c, i);
		printf("%s has $%d, and bet $%d\n", p->name, p->coins, p->bets);
		printf("%s has $%d, and bet $%d\n", c->name, c->coins, c->bets);

		/* begin */
		printf("\n=========== GAME START ==============\n");
		shuffle();	
		for(i=0;i<2;i++){
			getCard(p, dealCard()); 
			getCard(c, dealCard());
		}
		showCard(p, 1);
		showCard(c, 0);

		/* Take turns to do select */
		printf("\n=========== %s's TURNS ==============\n", p->name);
		while(1){
			showCard(p, 1);
			tmp = doSelect(p);
			if(tmp==1){
				printf("%s: HIT!!\n", p->name);
				tmp = getCard(p, dealCard());
				if(tmp==-1){
					showCard(p, 1);
					printf("%s: STAND!!\n", p->name);
					break;
				}
			}
			else if(tmp==2){
				printf("%s: STAND!!\n", p->name);
				tmp = doStand(p);
				if(tmp==0)
					break;
			}
			else if(tmp==3){
				printf("%s: DOUBLE!!\n", p->name);
				tmp = doDouble(p);
			}
		}
		printf("\n============ %s's TURNS ================\n", c->name);
		while(1){
			showCard(c, 0);
			tmp = autoSelect(c);
			if(tmp==1){
				printf("%s: HIT!!\n", c->name);
				tmp = getCard(c, dealCard());
				if(tmp==-1){
					showCard(c, 0);
					printf("%s: STAND!!\n", c->name);
					break;
				}
			}
			else if(tmp==2){
				printf("%s: STAND!!\n", c->name);
				tmp = doStand(c);
				if(tmp==0)
					break;
			}
			else if(tmp==3){
				printf("%s: DOUBLE!!\n", c->name);
				tmp = doDouble(c);
			}
		}
		/* Judge the winner and give the bet */
		printf("\n=========== JUDGE TIME =============\n");
		showCard(p, 1);
		showCard(c, 1);
		winner = judgeWinner(p, c);
		if(winner==NULL)
			printf("Tie!!\n");
		else
			printf("%s Win!!\n", winner->name);

		/* Judge play again or not */
		printf("\n=========== CONTINUE? ===============\n");
		printf("%s has $%d\n", p->name, p->coins);
		printf("%s has $%d\n", c->name, c->coins);

		if(p->coins<=0){
			printf("%s have no money. Game Over\n", p->name);
			break;
		}
		else if(c->coins<=0){
			printf("%s have no money. Game Over\n", c->name);
			break;
		}
		printf("==========================================================\n");	
	}
	return 0;
}

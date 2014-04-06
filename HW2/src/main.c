#include <stdio.h>
#include <stdlib.h>
#include "blackJack.h"
#include "message.h"
#include "pipe.h"
#include "sync.h"
#include "sema.h"

void normalVersion();
void showExe(char*);

int main(int argc, char *argv[]){
	if(argc>2)
		showExe(argv[0]);
	/* Play the game: myblackjack */
	else if(argc==1){
		printf("----------- Normal Version ------------\n");
		normalVersion();
	}
	/* pipe version: -p */
	else if(strcmp(argv[1], "-p")==0){
		printf("------------ Pipe Version -------------\n");
		pipeVersion();
	}
	/* message version: -m */
	else if(strcmp(argv[1], "-m")==0){
		printf("---------- Message Version ------------\n");
		messageVersion();
	}
	/* shared memory with semaphore version: -s */
	else if(strcmp(argv[1], "-s")==0){
		printf("----- Shared Memory with Semaphore -----\n");
		semaphoreVersion();
	}
	/* pthread with synchronization mechanism version: -t */
	else if(strcmp(argv[1], "-t")==0){
		printf("------- Synchronization Mechanism ------\n");
		pthreadVersion();	
	}
	/* Help function: myblackjack -h */
	else if(strcmp(argv[1], "-h")==0)
		showHelp();	
	else
		showExe(argv[0]);
	return 0;
}

void showExe(char *s){
	printf("%s [option]\n", s);
	printf("no option: Play the game with normal version. \n");
	printf(" -p: Pipe Version. \n");
	printf(" -m: Message Version. \n");
	printf(" -s: Shared Memory with Semaphore Version. \n");
	printf(" -t: Pthread with Synchromization Mechanism Version. \n");
	printf(" -h: The Rules. \n");
}

/* normal */
void normalVersion(){
	int i, tmp;
	player *p, *c, *winner;
	p = (player*)malloc(sizeof(player));
	c = (player*)malloc(sizeof(player));

	newPlayer(p, "Player", 10);
	newPlayer(c, "Computer", 10);
	newGame();
	while(1){
		/* Bet */
		do{
			printf("How much do %s want to bet? \n> ", p->name);
			scanf("%d", &i);
			if(i<=p->coins)
				break;
			printf("%s doesn't have enough money to bet $%d\n", p->name, i);
		}while(1);
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
		winner = judgeIni(p, c);
		if(winner!=NULL){
			showCard(p, 1);
			showCard(c, 1);
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
			continue;
		}

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
}

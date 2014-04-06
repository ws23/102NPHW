#include <stdio.h>                                                      
#include <stdlib.h>
#include <pthread.h>
#include "blackJack.h"
#include "sync.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

player *P = NULL;

void *t_func1(){
	pthread_cond_wait(&cond, &mutex);
	player *c, *winner; 
	int i, tmp;
	c = (player*)malloc(sizeof(player));
	pthread_cond_signal(&cond);
	pthread_cond_wait(&cond, &mutex);
	newPlayer(c, "Computer", 10);
	newGame();
	while(1){
		/* Bet */
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond,&mutex);
		i = autoBet(c);
		if(i==-1)
			break;
		iniPlayer(c, i);
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		printf("%s has $%d, and bet $%d\n", c->name, c->coins, c->bets);

		/* Begin */
		printf("\n============ GAME START ==============\n");
		shuffle();
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		getCard(c, dealCard());
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		getCard(c, dealCard());
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		showCard(c, 0);
		winner = judgeIni(c, P);
		if(winner!=NULL){
			printf("%s Win!!\n", winner->name);
			pthread_cond_signal(&cond);	
			
			showCard(c, 1);
			showCard(P, 1);
			printf("%s has $%d\n", c->name, c->coins);
			printf("%s has $%d\n", P->name, P->coins);
			if(c->coins<=0){
				printf("%s has no money. Game Over\n", c->name);
				break;
			}
			continue; 
		}
		P = winner;
		/* Take turns to do select */
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		printf("\n============== %s's TURNS ================\n", c->name);
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

		/* Judge the winner and give bet */
		printf("\n============ JUDGE TIME ==============\n");
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		showCard(c, 1);
		winner = judgeWinner(c, P);
		if(winner==NULL)
			printf("Tie!!\n");
		else
			printf("%s Win!!\n", winner->name);

		/* Judge play again or not */
		printf("\n========== CONTINUE? ==============\n");
		printf("%s has $%d\n", c->name, c->coins);
		P = c;
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);

		if(c->coins<=0){
			printf("%s has no money. Game Over\n", c->name);
			break;
		}	
		else if(P->coins<=0)
			break;
		pthread_cond_signal(&cond);
	}
	P = c;
	pthread_cond_signal(&cond);
	free(c);
	exit(0);
}

void *t_func2(){
	player *p;
	int i, tmp;
	p = (player*)malloc(sizeof(player));
	pthread_cond_signal(&cond);
	pthread_cond_wait(&cond, &mutex);
	newPlayer(p, "Player", 10);
	pthread_cond_signal(&cond);
	pthread_cond_wait(&cond, &mutex);
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
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		printf("%s has $%d, and bet $%d\n", p->name, p->coins, p->bets);
		/* Begin */
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		getCard(p, dealCard());
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		getCard(p, dealCard());
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		showCard(p, 1);
		P = p;
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		if(P!=NULL){
			if(p->coins<=0){
				printf("%s has no money. Game Over\n", p->name);
				break;
			}
			continue;
		}
		/*Take turns to do select*/
		printf("\n============= %s's TURNS ==============\n", p->name);
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
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);

		/* Judge the winner and give the bet */
		showCard(p, 1);
		P = p;
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		
		/* Judge play again or not */
		printf("%s has $%d\n", p->name, p->coins);
		pthread_cond_signal(&cond);
		pthread_cond_wait(&cond, &mutex);
		if(p->coins<=0){
			printf("%s has no money. Game Over\n", p->name)	;
			break;
		}
		else if(P->coins<=0)
			break;
		printf("========================================\n");
	}
	pthread_cond_signal(&cond);
	free(p);
	exit(0);
}

void pthreadVersion(){
	pthread_t thread1, thread2; 

	pthread_create(&thread1, NULL, &t_func1, NULL);
	pthread_create(&thread2, NULL, &t_func2, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	exit(0);
}

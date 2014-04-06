#include <stdio.h>                                                      
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "blackJack.h"
#include "sema.h"

#define KEY_VALUE 1
#define PERM 0666
#define NUMSEM 1
#define DUMMY 0


typedef struct _msg{
	int flag; 
	int value;
} msg; 

int semID, shmID;
msg *ptr; 

static struct sembuf lock = {0, 1, SEM_UNDO};
static struct sembuf unlock = {0, -1, SEM_UNDO | IPC_NOWAIT};

void s_parent(){
	player *c; 
	int i, tmp, winner, type;
	c = (player*)malloc(sizeof(player));
	newPlayer(c, "Computer", 10);
	newGame();
	semop(semID, &lock, 1);
	ptr->flag = 1;
	semop(semID, &unlock, 1);
	while(1){
		/* Bet */
		i = autoBet(c);
		if(i==-1)
			break;
		iniPlayer(c, i);
		printf("%s has $%d, and bet $%d\n", c->name, c->coins, c->bets);
		while(ptr->flag!=2);
		/* Begin */
		printf("\n============ GAME START ==============\n");
		shuffle();
		semop(semID, &lock, 1);
		ptr->flag = 3;
		ptr->value = dealCard();
		semop(semID, &unlock, 1);
		while(ptr->flag!=4);	
		tmp = dealCard();
		getCard(c, tmp);
		semop(semID, &lock, 1);
		ptr->flag = 5;
		ptr->value = dealCard();
		semop(semID, &unlock, 1);
		while(ptr->flag!=6);
		tmp = dealCard();
		getCard(c, tmp);
		showCard(c, 0);
		semop(semID, &lock, 1);
		ptr->flag = 7;
		ptr->value = tmp;
		semop(semID, &unlock, 1);	
		while(ptr->flag!=8);
		winner = judgeIniV(c->points, ptr->value);
		semop(semID, &lock, 1);
		ptr->flag = 9;
		ptr->value = winner; 
		semop(semID, &unlock, 1);
		if(winner==1){
			printf("%s Win!!\n", c->name);
			giveBet(c, 1);
		}
		if(winner!=0){
			showCard(c, 1);
			printf("%s has $%d\n", c->name, c->coins);
			if(c->coins<=0){
				printf("%s has no money. Game Over\n", c->name);
				break;
			}
			continue; 
		}

		/* Take turns to do select */
		type = 10;
		while(1){
			sleep(0.5);
			while(ptr->flag!=type); 
			type++;
			if(ptr->value==-1)
				break;
			semop(semID, &lock, 1);
			ptr->flag = type;
			type++;
			ptr->value = dealCard();
			semop(semID, &unlock, 1);
		}

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
		showCard(c, 1);
		semop(semID, &lock, 1);
		ptr->flag = type; 
		ptr->value = -1;
		type++;
		semop(semID, &unlock, 1);
		while(ptr->flag!=type);
		type++;
		tmp = ptr->value;
		while(ptr->flag!=type);
		type++;
		winner = judgeWinnerV(c->points, c->numCard, tmp, ptr->value);
		semop(semID, &lock, 1);
		ptr->flag = type; 
		type++;
		ptr->value = winner; 
		semop(semID, &unlock, 1);
		if(winner==0)
			printf("Tie!!\n");
		else if(winner==1 || winner==3){
			printf("%s Win!!\n", c->name);
			giveBet(c, winner/2+1);
		}

		/* Judge play again or not */
		printf("\n========== CONTINUE? ==============\n");
		while(ptr->flag!=type); 
		type++;
		printf("%s has $%d\n", c->name, c->coins);
		if(ptr->value<=0)
			break;
		semop(semID, &lock, 1);
		ptr->flag = type;
		type++;
		ptr->value = c->coins;
		semop(semID, &unlock, 1);	
		if(c->coins<=0){
			printf("%s has no money. Game Over\n", c->name);
			break;
		}
		printf("============================================\n");	
	}
	free(c);
}

void s_child(){
	player *p;
	int i, tmp, type;
	msg buf;
	p = (player*)malloc(sizeof(player));
	newPlayer(p, "Player", 10);
	while(ptr->flag!=1);
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
		printf("%s has $%d, and bet $%d\n", p->name, p->coins, p->bets);

		/* Begin */
		semop(semID, &lock, 1);
		ptr->flag = 2;
		semop(semID, &unlock, 1);
		while(ptr->flag!=3);
		getCard(p, ptr->value);
		semop(semID, &lock, 1);
		ptr->flag = 4;
		semop(semID, &unlock, 1);
		while(ptr->flag!=5);	
		getCard(p, ptr->value);
		semop(semID, &lock, 1);
		ptr->flag = 6;
		semop(semID, &unlock, 1);
		while(ptr->flag!=7);
		showCard(p, 1);
		semop(semID, &lock, 1);
		ptr->flag = 8;
		ptr->value = p->points; 
		semop(semID, &unlock, 1);
		while(ptr->flag!=9);
		if(ptr->value==2){
			printf("%s Win!!\n", p->name);
			giveBet(p, 1);
		}
		if(ptr->value!=0){
			showCard(p, 1);
			printf("%s has $%d\n", p->name, p->coins);
			if(p->coins<=0){
				printf("%s has no money. Game Over\n", p->name);
				break;
			}
			continue;
		}

		type = 10;
		/*Take turns to do select*/
		printf("\n============= %s's TURNS ==============\n", p->name);
		while(1){
			showCard(p, 1);
			tmp = doSelect(p);
			if(tmp==1){
				printf("%s: HIT!!\n", p->name);
				semop(semID, &lock, 1);
				ptr->flag = type;
				type++;
				ptr->value = 1;
				semop(semID, &unlock, 1);
				sleep(0.5);
				while(ptr->flag!=type);
				type++;
				sleep(0.5);
				tmp = getCard(p, ptr->value);
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
		semop(semID, &lock, 1);
		ptr->flag = type; 
		type++;
		ptr->value = -1;
		semop(semID, &unlock, 1);
		/* Judge the winner and give the bet */
		while(ptr->flag!=type);
		type++;
		showCard(p, 1);
		semop(semID, &lock, 1);
		ptr->flag = type;
		type++;
		ptr->value = p->points;
		sleep(0.5);
		ptr->flag = type;
		type++;
		ptr->value = p->numCard; 
		semop(semID, &unlock, 1);
	
		while(ptr->flag!=type);
		type++;
		if(ptr->value==2 || ptr->value==4){
			printf("%s Win!!\n", p->name);
			giveBet(p, ptr->value/2);
		}

		/* Judge play again or not */
		semop(semID, &lock, 1);
		ptr->flag = type;
		type++;
		ptr->value = p->coins; 
		semop(semID, &unlock, 1);
		printf("%s has $%d\n", p->name, p->coins);
		if(p->coins<=0){
			printf("%s has no money. Game Over\n", p->name)	;
			break;
		}
		while(ptr->flag!=type);
		type++;
		if(ptr->value<=0)
			break;
	}
	free(p);
}

void semaphoreVersion(){
	pid_t pid;

	semID = semget(KEY_VALUE, NUMSEM, IPC_CREAT | PERM); 
	semctl(semID, 0, SETVAL, 1);

	shmID = shmget(KEY_VALUE, sizeof(msg), IPC_CREAT | PERM);
	ptr = shmat(shmID, (char*)0, 0);

	ptr->flag = 0;
	ptr->value = -1;

	pid = fork();

	if(pid==0)
		s_child();
	else if(pid>0)
		s_parent();
	else{
		printf("fork error\n");
		exit(1);
	}
	semctl(semID, DUMMY, IPC_RMID, DUMMY);

	shmdt(ptr);
	shmctl(shmID, IPC_RMID, 0);
	exit(0);
}

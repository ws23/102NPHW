#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "blackJack.h"

void normal();
void pipe();
void message();
void semaphore();
void pthread();
void showExe(char*);

int main(int argc, char *argv[]){
	if(argc>2)
		showExe(argv[0]);
	/* Play the game: myblackjack */
	else if(argc==1){
		printf("----------- Normal Version ------------\n");
		normal();
	}
	/* pipe version: -p */
	else if(strcmp(argv[1], "-p")==0){
		printf("------------ Pipe Version -------------\n");
		pipe();
	}
	/* message version: -m */
	else if(strcmp(argv[1], "-m")==0){
		printf("---------- Message Version ------------\n");
		message();
	}
	/* shared memory with semaphore version: -s */
	else if(strcmp(argv[1], "-s")==0){
		printf("----- Shared Memory with Semaphore -----\n");
		semaphore();
	}
	/* pthread with synchronization mechanism version: -t */
	else if(strcmp(argv[1], "-t")==0){
		printf("------- Synchronization Mechanism ------\n");
		pthread();	
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
void normal(){
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
			printf("%s Win!!\n", winner->name);
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

/* pipe */

void p_parent(int fd1[], int fd2[]){
	int i, tmp;
	char buff[50];
	player *c;
	c = (player*)malloc(sizeof(player));
	newPlayer(c, "Computer", 10);
	newGame();
	while(1){
		/* Bet */
		tmp = autoBet(c);
		if(tmp==-1)
			exit(0);
		iniPlayer(c, tmp);
		printf("%s has $%d, and bet $%d\n", c->name, c->coins, c->bets);
	
		/* Begin */
		printf("\n================== GAME START ====================\n");	
		shuffle();
		

	}
}

void p_child(int fd1[], int fd2[]){
	int i, tmp;
	char buff[50];
	player *p;
	p = (player*)malloc(sizeof(player));
	newPlayer(p, "Player", 10);
	while(1){
		/* Bet */
		do{
			printf("How much %s want to bet \n> ", p->name);
			scanf("%d", &tmp);
			if(tmp<=p->coins)
				break;
			printf("%s doesn't have enough money to bet $%d\n", p->name, tmp);
		}while(1);
		iniPlayer(p, tmp);
		printf("%s has $%d, and bet $%d\n", p->name, p->coins, p->bets);
	}
}

void pipe(){
	pid_t pid;
	int fd1[2], fd2[2]; 	

	pipe(fd1);
	pipe(fd2);

	pid = fork();

	if(pid==0)
		p_child(fd1, fd2);
	else if(pid>0)
		p_parent(fd1, fd2);
	else
		printf("fork error\n");
}

/* message */

#define KEY_VALUE 1
#define PERM 0666

typedef struct _msg{
	int mType; 
	player *P;
} msg; 

void m_parant(){

}

void m_child(){

}

void message(){
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

/* shared memory with semaphore */

void semaphore(){
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


/* pthread with syncronization mechanism */

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
		if(P!=NULL)
			continue;
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

void pthread(){
	pthread_t thread1, thread2; 

	pthread_create(&thread1, NULL, &t_func1, NULL);
	pthread_create(&thread2, NULL, &t_func2, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	exit(0);
}

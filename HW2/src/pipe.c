#include <stdio.h>                                                      
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "blackJack.h"
#include "pipe.h"

int fd1[2], fd2[2];

void p_parent(){
	player *c; 
	int i, tmp, winner;
	char buf[20];
	close(fd1[0]);
	close(fd2[1]);
	c = (player*)malloc(sizeof(player));
	newPlayer(c, "Computer", 10);
	newGame();
	sprintf(buf, "%d", -1);
	write(fd1[1], buf, 20);
	while(1){
		/* Bet */
		i = autoBet(c);
		if(i==-1)
			break;
		iniPlayer(c, i);
		printf("%s has $%d, and bet $%d\n", c->name, c->coins, c->bets);
		read(fd2[0], buf, 20);
		/* Begin */
		printf("\n============ GAME START ==============\n");
		shuffle();
		sprintf(buf, "%d", dealCard());
		write(fd1[1], buf, 20);
		read(fd2[0], buf, 20);
		getCard(c, dealCard());
		sprintf(buf, "%d", dealCard());
		write(fd1[1], buf, 20);
		read(fd2[0], buf, 20);
		getCard(c, dealCard());
		showCard(c, 0);
		sprintf(buf, "%d", -2);
		write(fd1[1], buf, 20);
		read(fd2[0], buf, 20);
		winner = judgeIniV(c->points, atoi(buf));
		sprintf(buf, "%d", winner);
		write(fd1[1], buf, 20);
		if(winner==1){
			printf("%s Win!!\n", c->name);
			giveBet(c, 1);
		}
		if(winner!=0){
			showCard(c, 1);
			printf("%s has $%d\n", c->name, c->coins);
			if(c->coins<=0){
				printf("%s has no money. Game Over", c->name);
				break;
			}
			continue; 
		}

		/* Take turns to do select */
		while(1){
			read(fd2[0], buf, 20);
			if(atoi(buf)<0)
				break;
			sprintf(buf, "%d", dealCard());
			write(fd1[1], buf, 20);
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
		sprintf(buf, "%d", -3);
		write(fd1[1], buf, 20);
		read(fd2[0], buf, 20);
		tmp = atoi(buf);
		read(fd2[0], buf, 20);
		winner = judgeWinnerV(c->points, c->numCard, tmp, atoi(buf));
		sprintf(buf, "%d", winner);
		write(fd1[1], buf, 20);
		if(winner==0)
			printf("Tie!!\n");
		else if(winner==1 || winner==3){
			printf("%s Win!!\n", c->name);
			giveBet(c, winner/2+1);
		}

		/* Judge play again or not */
		printf("\n========== CONTINUE? ==============\n");
		read(fd2[0], buf, 20);
		printf("%s has $%d\n", c->name, c->coins);
		if(atoi(buf)<=0)
			break;
		sprintf(buf, "%d", c->coins);
		write(fd1[1], buf, 20);
		if(c->coins<=0){
			printf("%s has no money. Game Over\n", c->name);
			break;
		}
		printf("============================================\n");	
	}
	free(c);
}

void p_child(){
	player *p;
	int i, tmp;
	char buf[20];
	close(fd1[1]);
	close(fd2[0]);
	p = (player*)malloc(sizeof(player));
	newPlayer(p, "Player", 10);
	read(fd1[0], buf, 20);
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
		sprintf(buf, "%d", -1);
		write(fd2[1], buf, 20);
		read(fd1[0], buf, 20);
		getCard(p, atoi(buf));
		sprintf(buf, "%d", -2);
		write(fd2[1], buf, 20);
		read(fd1[0], buf, 20);
		getCard(p, atoi(buf));
		sprintf(buf, "%d", -2);
		write(fd2[1], buf, 20);
		read(fd1[0], buf, 20);
		showCard(p, 1);
		sprintf(buf, "%d", p->points);
		write(fd2[1], buf, 20);
		read(fd1[0], buf, 20);
		if(atoi(buf)==2)
			printf("%s Win!!\n", p->name);
		if(atoi(buf)!=0){
			showCard(p, 1);
			printf("%s has $%d.\n", p->name, p->coins);
			if(p->coins<=0){
				printf("%s has no money. Game Over.\n", p->name);
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
				sprintf(buf, "%d", 1);
				write(fd2[1], buf, 20);
				read(fd1[0], buf, 20);
				tmp = getCard(p, atoi(buf));
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
		sprintf(buf, "%d", -1);
		write(fd2[1], buf, 20);
		
		/* Judge the winner and give the bet */
		read(fd1[0], buf, 20);
		showCard(p, 1);
		sprintf(buf, "%d", p->points);
		write(fd2[1], buf, 20);
		sprintf(buf, "%d", p->numCard);
		write(fd2[1], buf, 20);

		read(fd1[0], buf, 20);
		if(atoi(buf)==2 || atoi(buf)==4){
			printf("%s Win!!\n", p->name);
			giveBet(p, atoi(buf)/2);
		}

		/* Judge play again or not */
		sprintf(buf, "%d", p->coins);
		write(fd2[1], buf, 20);
		printf("%s has $%d\n", p->name, p->coins);
		if(p->coins<=0){
			printf("%s has no money. Game Over\n", p->name)	;
			break;
		}
		read(fd1[0], buf, 20);
		if(atoi(buf)<=0)
			break;
	}
	free(p);
}

void pipeVersion(){
	pid_t pid;

	pipe(fd1);
	pipe(fd2);

	pid = fork();

	if(pid==0)
		p_child();
	else if(pid>0)
		p_parent();
	else
		printf("fork error\n");
	exit(0);
}

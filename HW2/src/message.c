#include <stdio.h>                                                      
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "blackJack.h"
#include "message.h"

#define KEY_VALUE 1
#define PERM 0666


typedef struct _msg{
	int mType; 
	int value;
} msg; 

int msgID; 

void m_parent(){
	player *c; 
	int i, tmp, winner, type;
	msg buf; 
	c = (player*)malloc(sizeof(player));
	newPlayer(c, "Computer", 10);
	newGame();
	buf.mType = 1;
	buf.value = -1;
	msgsnd(msgID, &buf, sizeof(msg), 0);
	while(1){
		/* Bet */
		i = autoBet(c);
		if(i==-1)
			break;
		iniPlayer(c, i);
		printf("%s has $%d, and bet $%d\n", c->name, c->coins, c->bets);
		msgrcv(msgID, &buf, sizeof(msg), 2, 0);
		/* Begin */
		printf("\n============ GAME START ==============\n");
		shuffle();
		buf.mType = 3;
		buf.value = dealCard();
		msgsnd(msgID, &buf, sizeof(msg), 0);
		msgrcv(msgID, &buf, sizeof(msg), 4, 0);
		tmp = dealCard();
		getCard(c, tmp);
		buf.mType = 5;
		buf.value = dealCard();
		msgsnd(msgID, &buf, sizeof(msg), 0);
		msgrcv(msgID, &buf, sizeof(msg), 6, 0);
		tmp = dealCard();
		getCard(c, tmp);
		showCard(c, 0);
		buf.mType = 7;
		buf.value = tmp;
		msgsnd(msgID, &buf, sizeof(msg), 0);
		msgrcv(msgID, &buf, sizeof(msg), 8, 0);
		winner = judgeIniV(c->points, buf.value);
		buf.mType = 9;
		buf.value = winner;
		msgsnd(msgID, &buf, sizeof(msg), 0);
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
			msgrcv(msgID, &buf, sizeof(msg), type, 0);
			type++;
			if(buf.value==-1)
				break;
			buf.mType = type;
			type++;
			buf.value = dealCard();
			msgsnd(msgID, &buf, sizeof(msg), type);
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
		buf.mType = type;
		type++;
		buf.value = -1;
		msgsnd(msgID, &buf, sizeof(msg), 0);
		msgrcv(msgID, &buf, sizeof(msg), type, 0);
		type++;
		tmp = buf.value;
		msgrcv(msgID, &buf, sizeof(msg), type, 0);
		type++;
		winner = judgeWinnerV(c->points, c->numCard, tmp, buf.value);
		buf.mType = type;
		type++;
		buf.value = winner; 
		msgsnd(msgID, &buf, sizeof(msg), 0);
		if(winner==0)
			printf("Tie!!\n");
		else if(winner==1 || winner==3){
			printf("%s Win!!\n", c->name);
			giveBet(c, winner/2+1);
		}

		/* Judge play again or not */
		printf("\n========== CONTINUE? ==============\n");
		msgrcv(msgID, &buf, sizeof(msg), type, 0);
		type++;
		printf("%s has $%d\n", c->name, c->coins);
		if(buf.value<=0)
			break;
		buf.mType = type;
		type++;
		buf.value = c->coins; 
		msgsnd(msgID, &buf, sizeof(msg), 0);
		if(c->coins<=0){
			printf("%s has no money. Game Over\n", c->name);
			break;
		}
		printf("============================================\n");	
	}
	free(c);
}

void m_child(){
	player *p;
	int i, tmp, type;
	msg buf;
	p = (player*)malloc(sizeof(player));
	newPlayer(p, "Player", 10);
	msgrcv(msgID, &buf, sizeof(msg), 1, 0);
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
		buf.mType = 2;
		buf.value = -1;
		msgsnd(msgID, &buf, sizeof(msg), 0);
		msgrcv(msgID, &buf, sizeof(msg), 3, 0);
		tmp = buf.value;
		getCard(p, tmp);
		buf.mType = 4;
		buf.value = tmp;
		msgsnd(msgID, &buf, sizeof(msg), 0);
		msgrcv(msgID, &buf, sizeof(msg), 5, 0);
		tmp = buf.value;
		getCard(p, tmp);
		buf.mType = 6;
		buf.value = tmp;
		msgsnd(msgID, &buf, sizeof(msg), 0);
		msgrcv(msgID, &buf, sizeof(msg), 7, 0);
		showCard(p, 1);
		buf.mType = 8;
		buf.value = p->points;
		msgsnd(msgID, &buf, sizeof(msg), 0);
		msgrcv(msgID, &buf, sizeof(msg), 9, 0);
		if(buf.value==2){
			printf("%s Win!!\n", p->name);
			giveBet(p, 1);
		}
		if(buf.value!=0){
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
				buf.mType = type;
				type++;
				buf.value = 1;
				msgsnd(msgID, &buf, sizeof(msg), 0);
				msgrcv(msgID, &buf, sizeof(msg), type, 0);
				type++;
				tmp = getCard(p, buf.value);
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
		buf.mType = type;
		type++;
		buf.value = -1;
		msgsnd(msgID, &buf, sizeof(msg), 0);

		/* Judge the winner and give the bet */
		msgrcv(msgID, &buf, sizeof(msg), type, 0);
		type++;
		showCard(p, 1);
		buf.mType = type;
		type++;
		buf.value = p->points;
		msgsnd(msgID, &buf, sizeof(msg), 0);
		buf.mType = type;
		type++;
		buf.value = p->numCard; 
		msgsnd(msgID, &buf, sizeof(msg), 0);

		msgrcv(msgID, &buf, sizeof(msg), type, 0);
		type++;
		if(buf.value==2 || buf.value==4){
			printf("%s Win!!\n", p->name);
			giveBet(p, buf.value/2);
		}

		/* Judge play again or not */
		buf.mType = type;
		type++;
		buf.value = p->coins;
		msgsnd(msgID, &buf, sizeof(msg), 0);
		printf("%s has $%d\n", p->name, p->coins);
		if(p->coins<=0){
			printf("%s has no money. Game Over\n", p->name)	;
			break;
		}
		msgrcv(msgID, &buf, sizeof(msg), type, 0);
		type++;
		if(buf.value<=0)
			break;
	}
	free(p);
}

void messageVersion(){
	pid_t pid;

	msgID = msgget(KEY_VALUE, IPC_CREAT | PERM);

	pid = fork();

	if(pid==0)
		m_child();
	else if(pid>0)
		m_parent();
	else{
		printf("fork error\n");
		exit(1);
	}
	msgctl(msgID, IPC_RMID, 0);
	exit(0);
}

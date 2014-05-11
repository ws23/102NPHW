#include <stdio.h>
#include <string.h>
#include "rpc_sc.h"
#include "blackJack.h"

int main(int argc, char *argv[]){
	CLIENT *cl;
	char *server; 
	int tmp, i; 
	msg message, *result;
	player *p, *c;
	if(argc!=2){
		printf("Argument error!\n Command: %s <ServerIP>\n", argv[0]);
		exit(1);
	}
	server = argv[1]; 

	cl = clnt_create(server, RPC_BJ, RPC_VERS, "tcp"); 
	if(cl==NULL){
		clnt_pcreateerror(server); 
		printf("Server connect error\n");
		exit(1); 
	}

	p = (player*)malloc(sizeof(player)); 
	c = (player*)malloc(sizeof(player));

	printf("How many coins do you have? \n> ");
	scanf("%d", &tmp);
	newPlayer(p, "Player", tmp);
	newPlayer(c, "Computer", 2147483647);

	// Hello
	memset(&message, 0, sizeof(message)); 
	message.Version_CommandType = (1u<<4) + 0u;
	message.Length = 5; 	
	message.Card[0] = p->coins; 
	message.Checksum = checksum(message);

	// Hello ACK
	result = hello_1(&message, cl); 
	if(result==NULL){
		clnt_perror(cl, server); 
		printf("Hello ACK error\n"); 
		exit(1); 
	}
	if(result->Checksum != checksum(*result)){
		printf("Hello ACK error\n"); 
		exit(1); 
	}
	if(result->Version_CommandType != (1u<<4) + 1u){
		printf("Hello ACK error\n"); 
		exit(1); 
	} 

	while(1){
		// START GAME request
		memset(&message, 0, sizeof(message))	; 
		message.Version_CommandType = (1u<<4) + 2u; 
		message.Length = 5; 
		do{
			printf("How much do you want to bet? \n>"); 
			scanf("%d", &tmp);
		}while(tmp>p->coins); 
		p->bets = tmp; 
		message.Card[0] = p->bets; 
		message.Checksum = checksum(message); 

		iniPlayer(p, message.Card[0]); 
		iniPlayer(c, 0);

		// START GAME response
		result = start_1(&message, cl); 
		if(result==NULL){
			clnt_perror(cl, server); 
			printf("START GAME ACK error\n"); 
			exit(1); 
		}
		if(result->Checksum != checksum(*result)){
			printf("START GAME ACK error\n"); 
			exit(1); 
		}
		if(result->Version_CommandType == (1u<<4) + 4u){
			printf("Server reject the START GAME request.\n"); 
			exit(0);
		}
		if(result->Version_CommandType != (1u<<4) + 3u){
			printf("START GAME ACK error\n"); 
			exit(1); 
		}
		getCard(p, result->Card[0]);
		getCard(c, result->Card[1]); 

		// SEND CARD request	
		memset(&message, 0, sizeof(message)); 
		message.Version_CommandType = (1u<<4) + 6u; 
		message.Length = 4; 
		message.Checksum = checksum(message); 

		// SEND CARD response
		result = send_1(&message, cl); 
		if(result==NULL){
			clnt_perror(cl, server);
			printf("SEND CARD ACK error\n"); 
			exit(1); 
		}
		if(result->Checksum != checksum(*result) || result->Version_CommandType != (1u<<4) + 5u){
			printf("SEND CARD ACK error\n"); 
			exit(1); 
		}
		getCard(p, result->Card[2]);
		getCard(c, result->Card[3]);
		showCard(p, 1); 
		showCard(c, 0); 


		// do select
		printf("\n=============== DO SELECT ==================\n"); 
		while(1){
			showCard(p, 1); 
			tmp = doSelect(p);
			if(tmp==1){
				printf("You HIT!!\n");
				// HIT request
				memset(&message, 0, sizeof(message));
				message.Version_CommandType = (1u<<4) + 9u; 
				message.Length = 4; 
				message.Checksum = checksum(message); 

				// HIT response
				result = hit_1(&message, cl); 
				if(result==NULL){
					clnt_perror(cl, server); 
					printf("HIT ACK error\n");
					exit(1); 
				}
				if(result->Checksum != checksum(*result) || result->Version_CommandType != (1u<<4) + 10u){
					printf("HIT ACK error\n");
					exit(1); 
				}
				for(i=0;i<result->Length-4-1;i++)
					p->cards[i] = result->Card[i];
				p->numCard = result->Length-4-1;
				tmp = getCard(p, result->Card[result->Length-4-1])	; 
				if(tmp==-1){
					showCard(p, 1); 
					printf("You STAND!!\n"); 
					break;
				}
			}
			else if(tmp==2){
				printf("You STAND!!\n"); 
				break;
			}		
			else if(tmp==3){
				printf("You DOUBLE!!\n"); 
				// DOUBLE request
				memset(&message, 0, sizeof(message)); 
				message.Version_CommandType = (1u<<4) + 7u; 
				message.Length = 4; 
				message.Checksum = checksum(message); 

				// DOUBLE response
				result = betdouble_1(&message, cl); 
				if(result==NULL){
					clnt_perror(cl, server); 
					printf("DOUBLE ACK error\n"); 
					exit(1); 
				}
				if(result->Checksum != checksum(*result) || result->Version_CommandType != (1u<<4) + 8u){
					printf("DOUBLE ACK error\n"); 
					exit(1); 
				}
				if(result->Card[0]==0)
					printf("Cannot DOUBLE!!\n"); 

			}
		}
		// STAND request
		memset(&message, 0, sizeof(message)); 
		message.Version_CommandType = (1u<<4) + 11u; 
		message.Length = 4; 
		message.Checksum = checksum(message); 

		// STAND response
		result = stand_1(&message, cl); 
		if(result==NULL){
			clnt_perror(cl, server); 
			printf("STAND ACK error\n"); 
			exit(1); 
		}
		if(result->Checksum != checksum(*result) || result->Version_CommandType != (1u<<4) + 12u){
			printf("STAND ACK error\n"); 
			exit(1); 
		}
		p->numCard = c->numCard = -1;

		for(i=0;i<5;i++){
			p->cards[i] = result->Card[1+i]; 
			c->cards[i] = result->Card[6+i];
			if(p->cards[i]==-1){
				if(p->cards[i-1]!=-1)
					p->numCard = i; 
			}
			if(c->cards[i]==-1){
				if(c->cards[i-1]!=-1)
					c->numCard = i; 
			}
		}
		if(p->numCard==-1)
			p->numCard = 5; 
		if(c->numCard==-1)
			c->numCard = 5; 

		showCard(p, 1); 
		showCard(c, 1); 

		if(result->Card[0]==0)
			printf("You win!!\n");
		else if(result->Card[0]==1)
			printf("You loss!!\n"); 
		else if(result->Card[0]==2)
			printf("Push!!\n"); 
		else{
			printf("STAND ACK error\n"); 
			exit(1); 
		}

		p->coins = result->Card[11]; 
		p->bets = result->Card[12]; 

		// ACK request
		memset(&message, 0, sizeof(message)); 
		message.Version_CommandType = (1u<<4) + 13u; 
		message.Length = 4; 
		message.Checksum = checksum(message); 

		// ACK response
		result = ack_1(&message, cl); 
		if(result==NULL){
			clnt_perror(cl, server); 
			printf("ACK's ACK error\n"); 
			exit(1); 
		}
		if(result->Checksum != checksum(*result) || result->Version_CommandType != (1u<<4) + 14u){
			printf("ACK's ACK error\n"); 
			exit(1);  
		}

		printf("You have %d coins. \n", p->coins); 
		if(p->coins<=0){
			printf("You don't have any money. Bye!!\n"); 
			exit(0); 
		}
		else{
			while(1){
				printf("Do you want to player again?[1. Yes/2. No] \n >"); 
				scanf("%d", &tmp); 
				if(tmp==1)
					break;	
				else if(tmp==2)
					exit(1); 	
				else
					printf("Input error\n"); 
			}
		}
	}
}

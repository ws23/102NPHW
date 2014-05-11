#include "rpc_sc.h"
#include "blackJack.h"

player *p, *c; 

msg* hello_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval; 
	printf("hello!\n");

	if(rcv->Checksum != checksum(*rcv))
		return NULL; 
	if(rcv->Version_CommandType != (1u<<4) + 0u)
		return NULL; 

	p = (player*)malloc(sizeof(player)); 
	c = (player*)malloc(sizeof(player)); 
	newPlayer(p, "Player", rcv->Card[0]); 
	newPlayer(c, "Computer", 2147483647); 
	newGame(); 	

	rval.Version_CommandType = (1u<<4) + 1u; 
	rval.Length = 4; 
	rval.Checksum = checksum(rval); 
	printf("complete hello\n");

	return &rval; 
}

msg* start_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval; 
	printf("start!!\n"); 

	if(rcv->Checksum != checksum(*rcv) || rcv->Version_CommandType != (1u<<4) + 2u){
		rval.Version_CommandType = (1u<<4) + 4u; 
		rval.Length = 4;
		rval.Checksum = checksum(rval); 
		return &rval; 
	}

	iniGame();
	iniPlayer(p, rcv->Card[0]);
	iniPlayer(c, 0);	
	
	shuffle(); 
	getCard(p, dealCard());
	getCard(c, dealCard());

	rval.Version_CommandType = (1u<<4) + 3u; 
	rval.Length = 6; 
	rval.Card[0] = p->cards[0];
	rval.Card[1] = c->cards[0]; 
	rval.Checksum = checksum(rval);
	printf("complete start\n");

	return &rval; 
}

msg* send_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval; 
	printf("send!!\n"); 

	if(rcv->Checksum != checksum(*rcv) || rcv->Version_CommandType != (1u<<4) + 6u){
		return NULL; 
	}

	getCard(p, dealCard());
	getCard(c, dealCard());
	
	rval.Version_CommandType = (1u<<4) + 5u; 
	rval.Length = 8; 
	rval.Card[0] = p->cards[0]; 
	rval.Card[1] = c->cards[0]; 
	rval.Card[2] = p->cards[1];
	rval.Card[3] = c->cards[1];	
	rval.Checksum = checksum(rval);
	printf("complete send\n");

	return &rval; 	
}

msg* betdouble_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval; 
	int tmp; 
	printf("double!!\n"); 

	if(rcv->Checksum != checksum(*rcv) || rcv->Version_CommandType != (1u<<4) + 7u)
		return NULL; 

	tmp = doDouble(p); 
	if(tmp==-1)
		rval.Card[0] = 0; 
	else
		rval.Card[0] = 1; 
	rval.Version_CommandType = (1u<<4) + 8u; 
	rval.Length = 4+1; 
	rval.Checksum = checksum(rval);
	
	printf("complete double\n");	
	return &rval; 	
}

msg* hit_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval;
	int i; 
	printf("hit!!\n"); 
	
	if(rcv->Checksum != checksum(*rcv) || rcv->Version_CommandType != (1u<<4) + 9u)
		return NULL; 

	getCard(p, dealCard());

	rval.Version_CommandType = (1u<<4) + 10u; 
	rval.Length = 4+p->numCard; 
	for(i=0;i<p->numCard;i++)
		rval.Card[i] = p->cards[i]; 
	rval.Checksum = checksum(rval); 

	printf("complete hit\n");

	return &rval; 
}

msg* stand_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval;
	int tmp, i; 
	player *winner; 
	printf("stand!!\n"); 

	if(rcv->Checksum != checksum(*rcv) || rcv->Version_CommandType != (1u<<4) + 11u)
		return NULL; 

	// Computer Action
	while(1){
		tmp = autoSelect(c); 
		if(tmp==1){
			tmp = getCard(c, dealCard());
			if(tmp==-1)
				break;
		}
		else if(tmp==2){
			tmp = doStand(c);
			if(tmp==0)
				break;
		}
	}

	// Jedge the winner and give the bet
	winner = judgeWinner(p, c);
	if(winner==NULL)	
		rval.Card[0] = 2; 
	else if(winner==p)
		rval.Card[0] = 0; 
	else
		rval.Card[0] = 1; 
	rval.Version_CommandType = (1u<<4) + 12u; 
	rval.Length = 4+1+10+2; 
	for(i=0;i<5;i++){
		rval.Card[1+i] = p->cards[i]; 
		rval.Card[6+i] = c->cards[i]; 
	}
	rval.Card[11] = p->coins; 
	rval.Card[12] = p->bets; 
	rval.Checksum = checksum(rval); 

	printf("complete stand\n");

	return &rval; 
}


msg* ack_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval;
	printf("ack!!\n"); 

	if(rcv->Checksum != checksum(*rcv) || rcv->Version_CommandType != (1u<<4) + 13u)
		return NULL; 

	rval.Version_CommandType = (1u<<4) + 14u; 
	rval.Length = 4; 
	rval.Checksum = checksum(rval);
	printf("complete ack\n");

	return &rval; 
}

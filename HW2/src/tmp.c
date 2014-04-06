void s_parent(){
	player *c, *winner; 
	int i, tmp;
	msg buf; 
	c = (player*)malloc(sizeof(player));
	newPlayer(c, "Computer", 10);
	newGame();
	while(1){
		/* Bet */
		i = autoBet(c);
		if(i==-1)
			break;
		iniPlayer(c, i);
		printf("%s has $%d, and bet $%d\n", c->name, c->coins, c->bets);

		/* Begin */
		printf("\n============ GAME START ==============\n");
		shuffle();
		getCard(c, dealCard());
		getCard(c, dealCard());
		showCard(c, 0);
		winner = judgeIni(c, P);
		if(winner!=NULL){
			printf("%s Win!!\n", winner->name);
			continue; 
		}
		P = winner;
		/* Take turns to do select */
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
		winner = judgeWinner(c, P);
		if(winner==NULL)
			printf("Tie!!\n");
		else
			printf("%s Win!!\n", winner->name);

		/* Judge play again or not */
		printf("\n========== CONTINUE? ==============\n");
		printf("%s has $%d\n", c->name, c->coins);
		P = c;

		if(c->coins<=0){
			printf("%s has no money. Game Over\n", c->name);
			break;
		}	
		else if(P->coins<=0)
			break;
	}
	P = c;
	free(c);
	exit(0);
}

void s_child(){
	player *p;
	int i, tmp;
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
		getCard(p, dealCard());
		getCard(p, dealCard());
		showCard(p, 1);
		P = p;
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

		/* Judge the winner and give the bet */
		showCard(p, 1);
		P = p;

		/* Judge play again or not */
		printf("%s has $%d\n", p->name, p->coins);
		if(p->coins<=0){
			printf("%s has no money. Game Over\n", p->name)	;
			break;
		}
		else if(P->coins<=0)
			break;
		printf("========================================\n");
	}
	free(p);
	exit(0);
}


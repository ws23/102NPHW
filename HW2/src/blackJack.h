/*	Card : 
 *	 0 ~ 12: Club A ~ Club K
 *	13 ~ 25: Diamond A ~ Diamond K
 *	26 ~ 38: Heart A ~ Heart K
 *	39 ~ 51: Spade A ~ Spade K
 */

typedef struct _pokerCard{
	int cards[52];
	int numCard;
} pokerCard;

typedef struct _player{
	char name[50]; 
	int coins;
	int bets; 
	int cards[5]; 
	int numCard; 
	int points; 
} player;  

// dealer
void newGame();  
void iniGame(); 
void shuffle(); 
int dealCard(); 
int doDouble(player*);
int doStand(player*);
player* judgeIni(player*, player*);
player* judgeWinner(player*, player*); 
void showHelp();

// player
void newPlayer(player*, char*, int);
void iniPlayer(player*, int); 
int autoBet(player*);
int againGame(); 
int getCard(player*, int); 
int doSelect(player*); 
int autoSelect(player*);
void showCard(player*, int);

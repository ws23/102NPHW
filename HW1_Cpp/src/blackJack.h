#include <iostream>
#include <string>
#define PLAYER 2
#define BET 10
#define SELF 0

using namespace std;

class blackJack{
	private:
		// Other variable
		int pCard[PLAYER][5];
		int pPoint[PLAYER];
		int pBet[PLAYER];
		int pbBet[PLAYER];
		int iniCard[52];
		string pName[PLAYER];
		int topCard;

		// function
		string getCard(int);
		int getPoint(int);
		int pOpt(int);
		int cOpt(int);
		int getTopCard();
		int setPoint(int);

	public:
		blackJack();
		void showHelp();
		void shuffle();
		int deal();
		string getCards(int);
		int getPoints(int);
		int getBet(int);
		int getbBet(int);
		int selectOpt(int);
		int judge();
};


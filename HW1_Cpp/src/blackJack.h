class poker{
	private:
		int card[52];
		int numCards;
	public:
		poker();
		int getCard(int);
		void shuffle();
		int dealCard();
		string showCard(int);
};

class player{
	private:
		int bet;
		int nbet;
		int card[5];
		int point;
		int numCards;
	public:
		player();
		void setCard(int);
		void setBet(int);
		void setNBet(int);
		void setPoint(int);
		int getCard();
		int getBet();
		int getNBet();
		int getPoint();
		string showCard();
		void selectOpt();
};

class computer:public player{
	public:
		computer();
		void selectOpt();
		string showCard();
};

class blackJack{
	private:
		player p;
		computer c;
		poker ini;
	public:
		blackJack();
		void startGame();
		void deal();
		int judge();
		int judgeWin();
		void processBet(int, int);
		void showHelp();
};

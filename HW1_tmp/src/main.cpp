#include "blackJack.h"
#include <cstring>

int main(int argc, char *argv[]){
	blackJack newGame;
	int tmp;
	if(argc>3 || (argc==2 && strcmp(argv[1], "-h")!=0))
		cout << "Argument error." << endl << "./myblackjack to play game. " << endl << "./myblackjack -h to read the rules." << endl;
	else if(argc==2)
		newGame.showHelp();

	// begin the game
	while(1){
		newGame.shuffle();
		tmp = newGame.deal();
		newGame.showCards();
		// if any player got 21 points, end this round. 
		if(tmp!=-1)
			;
		else{
			for(int i=0;i<PLAYER;i++)
				newGame.selectOpt(i);
			tmp = newGame.judge();
			//newGame.showCards();
			if(tmp<0)
				cout << "No one wins." << endl;
			else
				cout << newGame.getName(tmp) << " wins!!" << endl;
			tmp = -1;
			for(int i=0;i<PLAYER;i++)
				if(newGame.getBet(i)<=0)
					tmp = i;
			if(tmp!=-1){
				tmp = tmp==1?0:1;
				cout << newGame.getName(tmp) << " wins!! Congradulation!!" << endl << "End this game." << endl;
			}	
			else
				cout << "--------------------------------------------------" << endl;
		}
	}
}

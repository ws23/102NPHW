#include "blackJack.h"
#include <cstring>

int main(int argc, char *argv[]){
	blackJack newGame;
	int tmp;
	if(argc>3 || (argc==2 && strcmp(argv[1], "-h")!=0))
		cout << "Argument error." << endl << "./myblackjack to play game. " << endl << "./myblackjack -h to read the rules." << endl;
	else if(argc==2)
		showHelp();

	// begin the game
	while(1){
		newGame.shuffle();
		tmp = newGame.deal();
		// if any player got 21 points, end this round. 
		if(tmp==-2){
				
		}
		else if(tmp>-1){
		
		}
	}

}

#include <stdio.h>
#include <string.h>
#include "blackJack.h"

int main(int argc, char *argv[]){
	int tmp, i;
	if(argc>3 || (argc==2 && strcmp(argv[1], "-h")!=0))
		printf("Arguments Error.\n ./myblackjack to play game. \n ./myblackjack -h to read the rules. ");
	else if(argc==2)
		showHelp();	

	// begin the game
	while(1){
		deal();
		tmp =  judgeIni();
		if(tmp!=-1)
			continue;
		// do the option
		for(i=0;i<PLAYER;i++){
			while(1){
				tmp = selectOpt(i);
				if(tmp==-1)
					break;
			}			
		}
		judge();
		tmp = judgeWin();
		if(tmp!=-1)
			break;
		printf("==================================================\n");
		getchar();
	}
	return 0;
}

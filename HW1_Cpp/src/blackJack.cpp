#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include "blackJack.h"
using namespace std;

// class "poker"
poker::poker(){
	for(int i=0;i<52;i++)
		card[i] = i;
	numCards = 52;
	srand(time(NULL));
}

int poker::getCard(int cIndex){
	return card[cIndex];
}

void poker::shuffle(){
	int a, b, times;
	times = rand()%50000+10000;
	a = rand()%52;
	b = rand()%52;
	for(int i=0;i<times;i++){
		int tmp = card[a];
		card[a] = card[b];
		card[b] = tmp;
	}
}

int poker::dealCard(){
	numCards--;
	return card[numCards];
}

string poker::showCard(int cIndex){
	string tmp = "";
}

// class "player"
player::player(){
	bet = 10;
	nbet = 0;
	point = 0;
	numCards = 0;
	for(int i=0;i<5;i++)
		card[i] = -1;
}

void player::setCard(int cIndex){
	card[numCards] = cIndex;
	numCards++;
}

void player::setBet(int BET){
	bet = BET;
}

void player::setNBet(int NBET){
	nbet = NBET;
}

void player::setPoint(){
	int count = 0;
	for(int i=0;i<numCards;i++){
		



	}	
}

int player::getCard(){

}

int player::getBet(){

}

int player::getNBet(){

}

int player::getPoint(){

}

string player::showCard(){

}

void player::selectOpt(){

}

// class "computer"
computer::computer(){

}

void computer::selectOpt(){

}

string computer::showCard(){

}

// class "blackJack"
blackJack::blackJack(){

}

void blackJack::startGame(){

}

void blackJack::deal(){

}

int blackJack::judge(){

}

int blackJack::judgeWin(){

}

void blackJack::processBet(){

}

void blackJack::showHalp(){

}

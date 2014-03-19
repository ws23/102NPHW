#include <iostream>
#include <fstream>
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

string poker::showCard(int cID){
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
	bet -= nbet;
}

void player::setPoint(){
	int Ace = 0, count = 0;
	for(int i=0;i<numCards;i++){
		switch(card[i]%13){
			case 10:
			case 11:
			case 12:
				count += 10;
				break;
			case 1:
				count += 11;
				Ace++;
				break;
			default:
				count += card[i]%13+1;
		}		
		while(count>21 && Ace>0){
			Ace--;
			count -= 10;
		}	
	}	
	point = count;
}

int player::getBet(){
	return bet;
}

int player::getNBet(){
	return nbet;
}

int player::getPoint(){
	return point;
}

string player::showCard(){
	
}

void player::selectOpt(){
	
}

// class "computer"
computer::computer(){
	bet = 10;
	nbet = 0;
	point = 0;
	numCards = 0;
	for(int i=0;i<5;i++)
		card[i] = -1;
}

void computer::selectOpt(){

}

string computer::showCard(){

}

// class "blackJack"
blackJack::blackJack(){
	
}

void blackJack::startGame(){
	p.setNBet(1);
	c.setNBet(1);
	deal();
	p.selectOpt();
	c.selectOpt();
	judge();
}

void blackJack::deal(){
	ini.shuffle();
	for(int i=0;i<2;i++){
		p.setCard(ini.dealCard());
		c.setCard(ini.dealCard());
	}	

}

int blackJack::judge(){
	
}

int blackJack::judgeWin(){

}

void blackJack::processBet(void *winner, int ){

}

void blackJack::showHalp(){
	ifstream fin("Rules", ifstream::in);    
    string tmp;                                                                 
    while(fin.eof()){
	        fin >> tmp;
	        cout << tmp;
	    }
    fin.close();
}

#include "blackJack.h"
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
using namespace std;

// constructure
blackJack::blackJack(){
	for(int i=0;i<PLAYER;i++)
		pBet[i] = BET;
	pName[0] = "Player";
	pName[1] = "Computer";
}

// private funciton
string blackJack::getCard(int cIndex){
	string tmp = "";
	switch((int)cIndex/13){
		case 0:
			tmp += "♣ ";
			break;
		case 1:
			tmp += "♦ ";
			break;
		case 2:
			tmp += "♥ ";
			break;
		case 3:
			tmp += "♠ ";
			break;
	}
	switch(cIndex%13){
		case 0:
			tmp += "A";
			break;
		case 10:
			tmp += "J";
			break;
		case 11:
			tmp += "Q";
			break;
		case 12:
			tmp += "K";
			break;
		default:
			tmp += to_string(cIndex%13+1);
	}
	return tmp;
}

int blackJack::getPoint(int cIndex){
	int tmp;
	switch(cIndex%13){
		case 10:
		case 11:
		case 12:
			tmp = 10;
			break;
		case 0:
			tmp = 11;
			break;
		default:
			tmp = cIndex%13 + 1;
	}
	return tmp;
}

int blackJack::pOpt(int pIndex){
	int opt, i = 2;
	while(1){
		cout << "What do you want to do?" << endl;
		cout << "1) Hit" << endl;
		if(pPoint[pIndex]>=17)
			cout << "2) Stand" << endl;
		if(pbBet[pIndex]*2 <= pBet[pIndex])
			cout << "3) Double" << endl;
		cout << "> ";
		cin >> opt;
		if(opt==1 && i<5){
			for(;i<5;i++){
				if(pCard[pIndex][i]==-1){
					pCard[pIndex][i] = getTopCard();
					cout << "You got " << getCard(pCard[pIndex][i]) << "." << endl;
					break;
				}
			}	
			if(i==4)
				return -1;
		}
		else if(opt==2 && pPoint[pIndex]>=17){
			cout << "You Stand!!" << endl;
			return -2;
		}
		else if(opt==3 && pbBet[pIndex]*2<=pBet[pIndex]){
			pBet[pIndex] -= pbBet[pIndex]*2;
			pbBet[pIndex] *= 2;
			cout << "You bet " << pbBet[pIndex] << " now." << endl;
		}
		else
			cout << "Input error. Please try it again. " << endl;	
	}
}

int blackJack::cOpt(int pIndex){
	srand(time(NULL));
	int opt, i=2;
	while(1){
		opt = rand()%3+1;
		if(opt==1 && i<5){
			for(;i<5;i++){
				if(pCard[pIndex][i]==-1){
					pCard[pIndex][i] = getTopCard();
					cout << pName[pIndex] << " got " << getCard(pCard[pIndex][i]) << "." << endl;
					break;
				}
			}
			if(i==4)
				return -1;
		}
		else if(opt==2 && pPoint[pIndex]>=17){
			cout << pName[pIndex] << " Stand!!" << endl;
			return -2;
		}
		else if(opt==3 && pbBet[pIndex]*2<=pBet[pIndex]){
			pBet[pIndex] -= pbBet[pIndex]*2;
			pbBet[pIndex] *= 2;
			cout << pName[pIndex] << " Double!!" << endl;
		}
	}
}

int blackJack::getTopCard(){
	int tmp;
	tmp = iniCard[topCard];
	topCard++;
	return tmp;
}

void blackJack::setPoint(int pIndex){
	int tmp = 0, Ace = 0;
	for(int i=0;i<5;i++){
		if(pCard[pIndex][i]==-1)
			continue;
		if(pCard[pIndex][i]%13==0)
			Ace++;
		tmp += getPoint(pCard[pIndex][i]);
	}
	while(tmp>21 && Ace>0){
		tmp -= 10;
		Ace--;
	}
	pPoints[pIndex] = tmp;
}

// public funtion
void blackJack::showHelp(){
	ifstream fin("Rules");	
	string tmp;
	while(fin.eof()){
		fin >> tmp;
		cout << tmp;
	}
	fin.close();
}

void blackJack::shuffle(){
	// Initialization
	for(int i=0;i<52;i++)
		iniCard[i] = i;
	for(int i=0;i<PLAYER;i++){
		for(int j=0;j<5;j++)
			pCard[i][j] = -1;
		pPoint[i] = 0;
		pbBet[i] = 1;
	}
	topCard = 0;

	// shuffle
	srand(time(NULL));
	int times = rand()%50000+10000;
	for(int i=0;i<times;i++){
		int a = rand()%52;
		int b = rand()%52;
		int t = iniCard[a];
		iniCard[a] = iniCard[b];
		iniCard[b] = t;
	}
}

int blackJack::deal(){
	// deal
	for(int i=0;i<2;i++){
		for(int j=0;j<PLAYER;j++){
			pCard[i][j] = getTopCard();
			setPoint(j);
		}
	}
	
	// check if any player get 21 points
	int tmp, numWin = 0;
	for(int i=0;i<PLAYER;i++){
		if(getPoints(i)==21){
			numWin++;
			tmp = i;
		}
	}
	if(numWin==1)
		return tmp;
	else if(numWin>1)
		return -2;
	return -1;	
}

string blackJack::getCards(int pIndex){
	string tmp = getCard(pCard[pIndex][0]);
	for(int i=1;i<5;i++){
		if(pCard[pIndex][i]==-1)
			continue;
		tmp += "\t" + getCard(pCard[pIndex][i]);
	}
	return tmp;
}

int blackJack::getPoints(int pIndex){
	return pPoint[pIndex];
}

int blackJack::getBet(int pIndex){
	return pBet[pIndex];
}

int blackJack::getbBet(int pIndex){
	return pbBet[pIndex];
}

int blackJack::selectOpt(int pIndex){
	int tmp;
	if(pIndex==SELF)
		tmp = pOpt(pIndex);
	else
		tmp = cOpt(pIndex);
	return -1;
}

int blackJack::judge(){
	// check if any player get 5 cards
	int tmp, numWin = 0;
	for(int i=0;i<PLAYER;i++){
		if(pCard[i][4]!=-1 && pPoint[i]<=21){
			numWin++;
			tmp = i;
		}
	}
	if(numWin==1)
		return tmp;
	else if(numWin>=2)
		return -2;
	// get which player have the most point and does not bust
	int max = -1;
	for(int i=0;i<PLAYER;i++)
		if(pPoint[i]<=21 && pPoint[i]>max)
			max = i;
	for(int i=0;i<PLAYER;i++){
		if(pPoint[i]==max){
			numWin++;
			tmp;
		}
	}
	if(numWin==1)
		return tmp;
	else if(numWin>=2)
		return -2;
	return -1;
}


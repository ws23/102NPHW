main: src/main.o src/blackJack.o 
	g++ src/main.o src/blackJack.o -o ./myblackjack -O2 -Wall -Wshadow 
	rm -rf src/*.o src/*.gch
main.o: src/main.cpp
	g++ src/main.cpp -c

blackJack.o: src/blackJack.cpp src/blackJack.h
	g++ src/blackJack.cpp src/blackJack.h -c

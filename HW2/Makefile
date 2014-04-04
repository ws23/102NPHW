main: src/main.o src/blackJack.o 
	gcc src/blackJack.o src/main.o -o ./myblackjack -O2 -Wall -Wshadow
	rm -rf src/*.o src/*.gch
main.o: src/main.c
	gcc src/main.c -c

blackJack.o: src/blackJack.c src/blackJack.h
	gcc src/blackJack.c src/blackJack.h -c

main: src/five.o src/client.c src/server.c
	gcc src/five.o src/client.c -o Client -O2 -Wall -Wshadow -lpthread
	gcc src/five.o src/server.c -o Server -O2 -Wall -Wshadow 
	rm -rf src/*.o src/*.gch

five.o: src/five.c src/five.h
	gcc src/five.h src/five.c -c

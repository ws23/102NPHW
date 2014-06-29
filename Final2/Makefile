main: src/five.o src/client.c src/server.c
#	gcc src/five.o src/client.c -o Client1/clnt -O2 -Wall -Wshadow 
#	gcc src/five.o src/server.c -o Server/svr -O2 -Wall -Wshadow 
#	cp Client1/clnt Client2/clnt
	gcc src/client.c src/five.o -o client
	gcc src/server.c src/five.o -o server
	rm -rf src/*.o src/*.gch

five.o: src/five.c src/five.h
	gcc src/five.h src/five.c -c

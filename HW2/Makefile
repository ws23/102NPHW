main: src/main.o src/blackJack.o src/pipe.o src/sync.o src/message.o src/sema.o
	gcc src/blackJack.o src/pipe.o src/sync.o src/message.o src/sema.o src/main.o -o ./myblackjack -O2 -Wall -Wshadow -lpthread
	rm -rf src/*.o src/*.gch
main.o: src/main.c
	gcc src/main.c -c

blackJack.o: src/blackJack.c src/blackJack.h
	gcc src/blackJack.c src/blackJack.h -c

pipe.o: src/pipe.c src/pipe.h
	gcc src/pipe.c src/pipe.h -c

sync.o: src/sync.c src/sync.h
	gcc src/sync.c src/sync.h -c -lpthread

message.o: src/message.c src/message.h 
	gcc src/message.c src/message.h -c

sema.o: src/sema.c src/sema.h
	gcc src/sema.c src/sema.h -c


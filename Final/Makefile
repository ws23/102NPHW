main: src/five.o src/main.o
	gcc src/five.o src/main.o -o chess -O2 -Wall -Wshadow
	rm -rf src/*.o src/*.gch

main.o: src/main.c
	gcc src/main.c -c

five.o: src/five.c src/five.h
	gcc src/five.h src/five.c -c

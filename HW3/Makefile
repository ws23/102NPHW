main: src/blackJack.o src/rpc_sc.o src/rpc.h src/rpc_xdr.c src/rpc_clnt.c src/rpc_svc.c src/client.c src/server.c
	gcc src/blackJack.o src/rpc_sc.o src/rpc.h src/rpc_xdr.c src/rpc_clnt.c src/client.c -o rpc_client -O2 -Wall -Wshadow 
	gcc src/blackJack.o src/rpc_sc.o src/rpc.h src/rpc_xdr.c src/rpc_svc.c src/server.c -o rpc_server -O2 -Wall -Wshadow
	rm -rf src/*.o src/*.gch

blackJack.o: src/blackJack.c src/blackJack.h
	gcc src/blackJack.c src/blackJack.h -c

rpc_sc.o: src/rpc.h src/rpc_sc.h src/rpc_sc.c
	gcc src/rpc.h src/rpc_sc.c src/rpc_sc.h -c

#include <stdio.h> 
#include "rpc.h"

int main(int argc, char *argv[]){
	CLIENT *cl;
	char *server;
	msg message, *result;
	if( argc != 2 ){
		printf("Argument error!\n Command: %s ServerIP\n", argv[0]);
		exit(1);
	}
	server = argv[1];
	
	cl = clnt_create(server, RPC_GUESS, RPC_VERS, "tcp");
	if(cl==NULL){
		clnt_pcreateerror(server);
		printf("Server connect error\n");
		exit(1);
	}
	// Send Hello Message
	message.cmd = 0;
	result = hello_1(&message, cl);
	if(result==NULL){
		clnt_perror(cl, server);
		printf("Hello ACK error\n");
		exit(1);
	}
	if(result->cmd!=1){
		printf("Hello ACK error\n");
		exit(1);
	}

	// Send NewGame Message
	message.cmd = 2;
	result = newgame_1(&message, cl);
	if(result==NULL){
		clnt_perror(cl, server);
		printf("NewGame ACK error\n");
		exit(1);
	}
	if(result->cmd!=3){
		printf("NewGame ACK error\n");
		exit(1);
	}

	// Guess the Value
	while(1){
		message.cmd = 4;
		printf("Input the value that you guess.\n > ");
		scanf("%d", &message.data);
		result = guess_1(&message, cl);
		if(result==NULL){
			clnt_perror(cl, server);
			printf("Guess ACK error\n");
			exit(1);
		}
		if(result->cmd!=5){
			printf("Guess ACK error\n");
			exit(1);
		}
		printf("%dA%dB\n\n", result->data/10, result->data%10);
		if(result->data==40)
			break;
	}
	return 0;
}

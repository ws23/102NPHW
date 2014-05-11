#include <stdio.h>
#include "410121021_140401.h"

int main(int argc, char *argv[]){
	CLIENT *client; 
	char *server; 

	msg message, *result; 

	int n; 

	if(argc!=2){
		printf("Argument Error!!\n");
		printf("command: %s <host>\n", argv[0]);
		return 0;
	}
	server = argv[1];

	while(1){
		printf("Input a value. (<0 to exit)\n> ");
		scanf("%d", &n);
		if(n<0)
			exit(0);
		message.value = n;

		// Send the message
		client = clnt_create(server, RPC_PROG, RPC_VERS, "tcp");
		if(client==NULL){
			clnt_pcreateerror(server);
			exit(1);
		}

		// Get the receive message
		result = proc_1(&message, client);
		if(result==NULL){
			clnt_perror(client, server); 
			exit(1);
		}
		printf("fib(%d) = %d\n", n, result->value);
	}
	return 0;
}

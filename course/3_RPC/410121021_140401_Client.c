#include <stdio.h>
#include "rpc.h"

int main(int argc, char *argv[]){
	CLIENT *client; 
	char *server; 
	
	struct msg message, *result; 

	int n; 

	if(argc!=2){
		printf("Argument Error!!\n");
		printf("command: %s <host>\n", argv[0]);
		return 0;
	}

	server = argv[1];

	printf("Input a value. (<0 to exit)\n> ");
	scanf("%d", &n);
	message.
	


	return 0;
}

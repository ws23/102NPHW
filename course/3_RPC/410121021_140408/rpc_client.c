#include <stdio.h> 
#include "rpc.h"

int main(int argc,int *argv[])
{
	CLIENT *cl;
	char *server;
	msg message, *result;
	float *val; 
	if( argc != 3 )
	{
		fprintf(stderr, "command: %s \n", argv[0]);
		exit(1);
	}
	server = argv[1];
	message.value=atoi(argv[2]);
	printf("srv:%s\n msg:%d\n",server,message.value);
	cl = clnt_create(server, RPC_PROG, RPC_VERS, "tcp");
	if( cl == NULL )
	{
		clnt_pcreateerror(server);
		exit(1);
	}
	result = fib_1(&message, cl); // RPC call
	if( result == NULL )
	{ // RPC fail
		clnt_perror(cl, server);
		exit(1);
	}
	printf("1.The return message is %d\n",result->value); 
	val = square_1(&message, cl); // RPC call
	if( val == NULL )
	{ // RPC fail
		clnt_perror(cl, server);
		exit(1);
	}
	printf("2.The return message is %f\n",*val); 
}

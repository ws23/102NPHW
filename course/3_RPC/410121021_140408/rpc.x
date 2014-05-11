struct msg{
	int value; 
};

program RPC_PROG{
	version RPC_VERS{
		float square(msg) = 1;
		msg fib(msg) = 2;
	} = 1; 
} = 1; 



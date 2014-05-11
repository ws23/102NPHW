struct msg{
	int value; 
};

program RPC_PROG{
	version RPC_VERS{
		msg PROC(msg) = 1;
	} = 1; 
} = 1; 



struct msg{
	int cmd; 
	int data; 
};

typedef struct msg msg; 

program RPC_GUESS{
	version RPC_VERS{
		msg hello(msg) = 0;
		msg newgame(msg) = 1;
		msg guess(msg) = 2;
	} = 1;
} = 0x1;

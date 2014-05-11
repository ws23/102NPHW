struct msg{
	uint8_t Version_CommandType; 
	uint8_t Length; 
	uint16_t Checksum;
	int8_t Card[128];
};

typedef struct msg msg; 

program RPC_BJ{
	version RPC_VERS{
		msg hello(msg) = 0;
		msg start(msg) = 1; 
		msg send(msg) = 2; 
		msg betdouble(msg) = 3; 
		msg hit(msg) = 4; 
		msg stand(msg) = 5;
		msg ack(msg) = 6;	
	} = 1; 
} = 0x1; 

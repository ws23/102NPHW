#include "rpc.h"

int f(int x){
	static i=2, f[100] = {0, 1};
	if(i==2){
		for(;i<100;i++)
			f[i] = f[i-1] + f[i-2];
	}
	return f[x];
}

msg *fib_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg snd;
	printf("server get: %d, ", rcv->value);
	snd.value = f(rcv->value);
	printf("Server calculate get: %d\n", snd.value);
	return &snd;	
}

float *square_1_svc(msg *rcv, struct svc_req *rqstp){
	static float snd; 
	printf("server get: %d, ", rcv->value);
	snd = rcv->value*rcv->value; 
	printf("server calculate get: %f\n", snd);
	return &snd;
}


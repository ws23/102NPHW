#include "410121021_140401.h"

int f(int x){
	static i=2, f[100] = {0, 1};
	if(i==2){
		for(;i<100;i++)
			f[i] = f[i-1] + f[i-2];
	}
	return f[x];
}

msg *proc_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg snd;
	printf("server get: %d, ", rcv->value);
	snd.value = f(rcv->value);
	printf("Server calculte get: %d\n", snd.value);
	return (&snd);	
}


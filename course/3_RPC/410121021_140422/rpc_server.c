#include "rpc.h"

static int guessword = 0;

msg* hello_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval; 
	if(rcv->cmd==0)
		rval.cmd = 1;
	else
		rval.cmd = 0;
	return &rval;
}

msg* newgame_1_svc(msg *rcv, struct svc_req *rqstp){
	static msg rval; 
	int randval = 0, tmp = 0, i = 0;
	tmp = randval = guessword = 0;
	if(rcv->cmd==2){
		guessword = rand()%10;
		for(i=0;i<3;i++){
			tmp = guessword;
			randval = rand()%10;
			do{
				while(randval==tmp%10)
					randval = rand()%10;
				tmp = tmp/10;
			}while(tmp>0);
			guessword = guessword*10+randval; 
		}
		rval.cmd = 3;
		rval.data = guessword;
	}
	return &rval;
}

msg *guess_1_svc(msg *rcv, struct svc_req *rqstp){
	static struct msg rval; 
	int a, b, c, d, ga, gb, gc, gd; 
	int cA, cB;
	printf("Player guess %04d\n", rcv->data);
	d = rcv->data%10;
	c = (rcv->data/10)%10;
	b = (rcv->data/100)%10;
	a = rcv->data/1000;
	cA = cB = 0;
	gd = guessword%10;
	gc = (guessword/10)%10;
	gb = (guessword/100)%10;
	ga = guessword/1000;
	cA += (a == ga?1:0);
	cA += (b == gb?1:0);
	cA += (c == gc?1:0);
	cA += (d == gd?1:0);
	if(a==gb||a==gc||a==gd)
		cB++;
	if(b==ga||b==gc||b==gd)
		cB++;
	if(c==gb||c==ga||c==gd)
		cB++;
	if(d==gb||d==gc||d==ga)
		cB++;
	rval.cmd = 5;
	rval.data = cA*10+cB;
	return &rval;

}

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define KEY_VALUE 1
#define PERM 0666

typedef struct _message{
	int mType;
	int value;
}message;

int f(int x){
	int i = 2;
	static int fib[100] = {0, 1};
	if(i==2)
		for(;i<100;i++)
			fib[i] = fib[i-1] + fib[i-2];
	return fib[x]; 
}

int main(){
	int msgID, n;
	pid_t pid;
	message buf;

	msgID = msgget(KEY_VALUE, IPC_CREAT | PERM);

	pid = fork();

	if(pid==0){
		while(1){
			printf("This is child process.\n");
			printf("Input a number(<0 to exit): ");
			scanf("%d", &n);
			buf.mType = 1;
			buf.value = n;

			msgsnd(msgID, &buf, sizeof(message), 0);
			printf("Child: Send %d. \n", buf.value);

			if(n<0)
				break;

			msgrcv(msgID, &buf, sizeof(message), 2, 0);
			printf("Child: Get %d.\n", buf.value);

			printf("fib = %d\n", buf.value);
		}
	}
	else if(pid>0){
		while(1){
			printf("This is parent process.\n");
			msgrcv(msgID, &buf, sizeof(message), 1, 0);
			printf("Parent: Get %d.\n", buf.value);
			n = buf.value;
	
			if(n<0)
				break;
			buf.mType = 2;
			buf.value = f(n);
			msgsnd(msgID, &buf, sizeof(message), 0);
			printf("Parent: Send %d.\n", buf.value);	
		}
	}
	else
		printf("fork error\n");

	msgctl(msgID, IPC_RMID, 0);
	return 0;
}

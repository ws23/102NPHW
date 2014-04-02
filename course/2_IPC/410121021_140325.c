#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define KEY_VALUE 1
#define PERM 0666
#define NUMSEM 1
#define DUMMY 0

typedef struct _ans{
	int flag; 
	int value;
}ans;

int fib(int x){
	static i=2, f[100] = {0, 1};
	if(i==2){
		for(;i<100;i++)
			f[i] = f[i-1] + f[i-2];
	}
	return f[x];
}

int main(){
	int n, tmp, semid, shmid;
	ans *shm_ptr;
	pid_t pid;
	static struct sembuf lock = {0, 1, SEM_UNDO};
	static struct sembuf unlock = {0, -1, SEM_UNDO | IPC_NOWAIT};
	
	if((semid = semget(KEY_VALUE, NUMSEM, IPC_CREAT | PERM))==-1){
		printf("semget error\n");
		exit(1);
	}
	if((semctl(semid, 0, SETVAL, 1))==-1){
		printf("semctl error\n");
		exit(1);
	}

	if((shmid = shmget(KEY_VALUE, sizeof(ans), IPC_CREAT | PERM))==-1){
		printf("shmget error\n");
		exit(1);
	}
	if((shm_ptr = shmat(shmid, (char*)0, 0))==(char*)-1){
		printf("shmat error\n");
		exit(1);
	}

	shm_ptr->flag = 1;

	pid = fork();
	if(pid==0){
		printf("This is the child process.\n");	
		while(1){
			while(shm_ptr->flag==0);
			if(semop(semid, &lock, 1)==-1){
				printf("C1: lock error\n");
				exit(1);
			}
			printf("C: Input a value to calculate the fib num. (<0 to exit)\n");
			scanf("%d", &n);
			shm_ptr->value = n;
			shm_ptr->flag = 0;
			if(semop(semid, &unlock, 1)==-1){
				printf("C1: unlock error\n");
				exit(1);
			}
			if(n<0)
				exit(0);
			while(shm_ptr->flag==0);
			if(semop(semid, &lock, 1)==-1){
				printf("C2: lock error\n");
				exit(1);
			}
			tmp = shm_ptr->value;
			printf("C: fib(%d) = %d\n", n, tmp);
			shm_ptr->flag = 0;
			if(semop(semid, &unlock, 1)==-1){
				printf("C2: unlock error\n");
				exit(1);
			}
		}
	}
	else if(pid>0){
		printf("This is the parent process.\n");
		while(1){
			while(shm_ptr->flag==1);
			if(semop(semid, &lock, 1)==-1){
				printf("P: lock error\n");
				exit(1);
			}
			n = shm_ptr->value;
			if(n<0){
				if(semctl(semid, DUMMY, IPC_RMID, DUMMY)==-1){
					printf("P: semctl remove error\n");
					exit(1);
				}
				shmdt(shm_ptr);
				shmctl(shmid, IPC_RMID, 0);
				exit(0);
			}
			tmp = fib(n);
			shm_ptr->value = tmp;
			shm_ptr->flag = 1;
			if(semop(semid, &unlock, 1)==-1){
				printf("P: unlock error\n");
				exit(1);
			}
		}
	}
	else{
		printf("fork error\n");
		exit(1);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

void* func1();
void* func2();
int fib(int);

int n = 0;

int main(){
	pthread_t thread1, thread2;

	pthread_create(&thread1, NULL, &func1, NULL);
	pthread_create(&thread2, NULL, &func2, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	
	exit(0);
}

int fib(int x){
	static int f[100] = {0, 1};
	int i;
	for(i=2;i<=x;i++)
		f[i] = f[i-1] + f[i-2];
	return f[x];	
}

void *func1(){
	while(1){
		pthread_mutex_lock(&count_mutex);

		pthread_cond_wait(&condition_var, &count_mutex);
	
		printf("f(%d) = %d\n", n, fib(n));

		pthread_cond_signal(&condition_var);
		pthread_mutex_unlock(&count_mutex);
		if(n<=0)
			exit(0);
	}
}

void *func2(){
	while(1){
		pthread_mutex_lock(&count_mutex);
	
		printf("Input the number. (<0 to exit): ");
		scanf("%d", &n);

		if(n>=0)
			pthread_cond_signal(&condition_var);
		else
			exit(0);
		pthread_cond_wait(&condition_var, &count_mutex);
		pthread_mutex_unlock(&count_mutex);	
	}
}

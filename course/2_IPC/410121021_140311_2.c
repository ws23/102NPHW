#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
	char buff[256];
	FILE *fd = popen("ps -aux", "r");
	while(fgets(buff, 256, fd)!=NULL)
		printf("%s", buff);
	pclose(fd);	
}


#include <stdio.h>
#include <ctype.h>

int main(){
	char input[5]; 
	int hex[4], binary[16], output[16]; 
   	int i;  

	while(scanf("%s", input)!=EOF){
		for(i=0;i<4;i++){
			if(isdigit(input[i]))
				hex[i] = input[i]-'0'; 	
			else if(input[i]>='A' && input[i]<='F')
				hex[i] = input[i]-'A'+10;	
			else if(input[i]>='a' && input[i]<='f')
				hex[i] = input[i]-'a'+10;
			else{
				printf("Input[%d](%c) Error!!\n", i, input[i]);
				i = -1;
				break;
			}
		}
		if(i==-1)
			continue; 
	
		for(i=0;i<4;i++){
			binary[i*4+3] = hex[i]%2; 
			binary[i*4+2] = (hex[i]/2)%2; 
			binary[i*4+1] =	((hex[i]/2)/2)%2; 
			binary[i*4] = ((hex[i]/2)/2)/2; 
		}
		
		for(i=0;i<16;i++){
			output[i] = binary[i]==1?0:1; 
			printf("%d", output[i]);
		}
		printf("\n");
	}


	return 0;
}

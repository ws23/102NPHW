#include <stdint.h>

typedef struct packet{
	uint8_t cmd, length; 
	uint16_t checksum; 
	int8_t data[254]; 
}pack; 

int clientNum; 

int chess(int, int, int);	// who, x, y
void chat(int, char*);		// who, message
void show(); 
void ini(); 
int judge(); 
uint16_t checksum(pack); 
void Log(char*, char);		// message, type

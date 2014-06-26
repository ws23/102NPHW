#include <stdint.h>

typedef struct packet pack; 

int clientNum; 

int chess(int, int, int);	// who, x, y
void chat(int, char*);		// who, message
void show(); 
uint16_t checksum(pack); 
void Log(char*, char);		// message, type

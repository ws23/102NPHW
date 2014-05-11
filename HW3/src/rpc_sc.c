#include "rpc_sc.h"

uint16_t checksum(msg pack){
    // add groups
    uint8_t hex[4]; 
    int8_t i, length; 
	uint16_t check; 
    hex[0] = pack.Version_CommandType>>4; 
    hex[1] = (pack.Version_CommandType<<4)>>4;
    hex[2] = pack.Length>>4; 
    hex[3] = (pack.Length<<4)>>4;   
	length = pack.Length; 

    for(i=0;i<length-4;i+=2){
        hex[0] += pack.Card[i]>>4; 
        hex[1] += (pack.Card[i]<<4)>>4; 
        hex[2] += pack.Card[i+1]>>4; 
        hex[3] += (pack.Card[i+1]<<4)>>4;
        if(hex[3]>15u){
            hex[3] -= 16u; 
            hex[2] += 1u;
        }
        if(hex[2]>15u){
            hex[2] -= 16u; 
            hex[1] += 1u; 
        }
        if(hex[1]>15u){
            hex[1] -= 16u; 
            hex[0] += 1u; 
        }
        if(hex[0]>15u){
            hex[0] -= 16u; 
            hex[3] += 1u;       
        }
    }                                                                   
    check = hex[0]<<12 + hex[1]<<8 + hex[2]<<4 + hex[3]; 
    check = ~check;
	return check; 
}

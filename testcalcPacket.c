#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "calcPacket.h"


int main(int argc,char* argv[])
{

	uint8_t i;
    unsigned char command[BUFFER_SIZE];
	strcpy((char*)command,argv[1]);  // command to send to a GPIB device
	printf("Input string %s\n",command);

	packet(command,strlen(command));
    for (i=0;i<strlen(command);i++){
        printf("%02x ",command[i]);
    }

	printf("\n");
	

	printf("Payload \n");
    for (i=0;i<strlen(payload);i++){
        printf("%02x ",payload[i]);
    }
    printf("\n");

    return 0;
}

/*
 */

#include <stdio.h>
#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"



int main(int argc,char* argv[])
{

	unsigned short gpibStatus;
	unsigned short chan,i;
	
// open and configure the serial port
	initializeBoard();

if (argc==2){
	
	chan=(unsigned short)strtol(argv[1],NULL,16);


	getRS485GPIBStatus(&gpibStatus,chan);


	printf("Return STATUS for GPIB bridge device on RS485 address  %02x: %02x\n\n",chan,gpibStatus);

	printf("REN\tEOI\tDAV\tNRFD\tNDAC\tIFC\tSRQ\tATN\n");

	for (i=0;i<8;i++){

		printf("%01x\t",((gpibStatus & (0x01 << i)) >> i));
	}

}


       printf("\n OK.\n");

// exit normally
    return 0;
}

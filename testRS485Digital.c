/*
 */

#include <stdio.h>
#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"



int main(int argc,char* argv[])
{


	unsigned short chan,value;
	int status;


// open and configure the serial port
	initializeBoard();


if (argc==3){

	value=(unsigned short)strtol(argv[1],NULL,16);
	chan=(unsigned short)strtol(argv[2],NULL,16);

	printf("Setting: %02X to address %02X\n",value,chan);
	status=setRS485DigitalOut(chan,value);
	printf("Status %d\n",status);
//	status=getRS485DigitalOut(chan,&value);
//	printf("Confirm: %02X\n",value);
//	printf("Status %d\n",status);

} else printf("usage: ./testRS485Digtal <valueHEX> <AddressHEX>\n");




// exit normally
    return 0;
}

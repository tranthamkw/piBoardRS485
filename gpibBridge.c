/*
 */

#include <stdio.h>
#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"



int main(int argc,char* argv[])
{


	unsigned char cmd[3], command[64],returnData[64];
	unsigned short chan,gpib;
	int status;
	unsigned char terminator;


// open and configure the serial port
	initializeBoard();

if (argc==5){

	strcpy((char*)command,argv[1]);  // command to send to a GPIB device
	terminator=(unsigned short)strtol(argv[2],NULL,16);
	chan=(unsigned short)strtol(argv[3],NULL,16);
	gpib=(unsigned short)strtol(argv[4],NULL,16);

	printf("RS485 to GPIB Bridge: RS485  Address %02x = %d\n",chan,chan);
	printf("Address of GPIB device attached to bridge %02x = %d\n",gpib,gpib);

	resetGPIBBridge(chan);
	printf("Sending IFC\n");
	delay(500);
	
	status = strlen(command);
	command[status] = terminator;
	command[status+1]=0;

	printf("Sending \n %s\n to GPIB device %d\n",command,gpib);
	status=sendGPIBData(command,gpib,chan);
	printf("Status %d\n",status);
	delay(500);

	printf("Listening\n");
	status=listenGPIBData(returnData,terminator,gpib,chan);
	printf("Status %d\n",status);
	printf("return string %s\n",returnData);

printf("ok\n");
}else {
printf("usage ./gpibBridge \"texttosend\" <terminator> <HEX RS485Address of bridge> <HEX GPIB address on bus> \n");
printf("\n e.g.\n ./gpibBridge \"R1X\" 0D C4 0A\n");
printf("common terminators LF = 0A, CR = 0D\n");


}

// exit normally
    return 0;
}


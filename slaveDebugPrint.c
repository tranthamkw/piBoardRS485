/*

asdf
 */

#include <stdio.h>
#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"



int main(int argc,char* argv[])
{

	unsigned short chan;
	unsigned short debug;
	int status;

// open and configure the serial port
	initializeBoard();

if (argc==3){
	// usage ./slaveDebugPrint <485AddHEX> <debug - 0 or 1>
	chan=(unsigned short)strtol(argv[1],NULL,16);
	debug=(unsigned short)strtol(argv[2],NULL,16);

	status=setRS485BridgeDebugPrint(debug,chan);

	printf("status: %d\n",status);
}else{
printf("usage ./slaveDebugPrint <485AddHex> <debug 0 or 1> \n");
}
       printf("OK.\n");

// exit normally
    return 0;
}

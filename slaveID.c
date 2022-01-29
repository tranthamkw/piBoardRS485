/*
edit by pi 4


asdf
 */

#include <stdio.h>
#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"



int main(int argc,char* argv[])
{

	unsigned char echoData[128];
	unsigned short chan;
	unsigned short newchan;
	int status;

// open and configure the serial port
	initializeBoard();

if (argc==2){
	// usgage ./slaveID <485Add HEX>
	chan=(unsigned short)strtol(argv[1],NULL,16);
	getRS485SlaveID(echoData,chan);
	printf("Return String for device %02x: %s\n",chan,echoData);

} else { if (argc==3){
	// usage ./slaveID <Present485AddHEX> <New485AddHEX>
	chan=(unsigned short)strtol(argv[1],NULL,16);
	newchan=(unsigned short)strtol(argv[2],NULL,16);
	status=changeRS485Address(chan,newchan);
	printf("status: %d\n",status);

	}

}


       printf("OK.\n");

// exit normally
    return 0;
}

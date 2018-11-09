/*
 */

#include <stdio.h>
#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"



int main(int argc,char* argv[])
{


	unsigned char command[64];
	unsigned char echoData[128];
	unsigned short chan,reads,timeout;
	int status;


// open and configure the serial port
	initializeBoard();


if (argc==3){

	strcpy((char*)command,argv[1]);
	chan=(unsigned short)strtol(argv[2],NULL,16);
/*
	status=getRS485BridgeReads(&reads,chan);
	printf("Bridge at address %02X reads %d times before sending data back\n",chan,reads);
	printf("Status %d\n",status);

	status=getRS485BridgeTimeout(&timeout,chan);
	printf("Bridge at address %02X has a timeout value of %04X\n",chan,timeout);
	printf("Status %d\n",status);

*/
	printf("RS485 Addres %02x = %d\n",chan,chan);
	printf("Sending %s\n",command);
	writeRS485to232Bridge(command,echoData,chan);
	printf("Return: %s\n",echoData);

}else{
printf("usage ./winBridge \"Command to send\" <HEX RS485 bridge address>\n");
}

       printf("OK.\n");

// exit normally
    return 0;
}

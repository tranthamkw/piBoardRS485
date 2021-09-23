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
	int status,i;


// open and configure the serial port
	initializeBoard();


if (argc==3){

	strcpy((char*)command,argv[1]);
	chan=(unsigned short)strtol(argv[2],NULL,16);

	i = strlen(command);
	//0x0a = line feed
	//0x0d = carriage return
	command[i]=0x0D;
	command[i+1]=0;

	printf("RS485 Addres %02x = %d\n",chan,chan);
	printf("Sending %s\n",command);

	writeRS485to232Bridge(command,echoData,chan);

	printf("Return: %s\n",echoData);

}else{
printf("usage ./RS232Bridge \"Command to send\" <HEX RS485 bridge address>\n");
}

       printf("OK.\n");

// exit normally
    return 0;
}

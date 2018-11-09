/*
program to manually compose a Modbus RTU message to be sent over the RS485 bus
usage: $~ sudo ./manualRTC xx yy zz ...
where  xx, yy, zz are HEXIDECIMAL representations of each 8-bit byte, in order to be written to bus
example $~ sudo ./manual 05 0A FF
the CRC is automatically appended to the end of the byte array.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piBoard.h"



int main (int argc, char* argv[]){


char returndata[64];
unsigned char cmd[16];  //still use char arrays since this is convient. do not do c-style string operations
// on this variable since it will not necessarily null terminated.
int i,j;
unsigned short num;
	unsigned short temp;


initializeBoard();
//	initialize_rs485(9600,25);
	//9600 is the default for most equipment


printf("Input (hex): ");
if (argc>2){
	for (i=0; i<(argc-1); i++){
		num=(unsigned short)strtol(argv[i+1],NULL,16);
		printf("%02x  ",num);
		cmd[i]=num;

	}
	temp = modRTU_CRC(cmd,i);
//	printf("\t crc %04x ",temp);
	cmd[i+1]=(unsigned char)((temp&0xFF00)>>8);  //ensures that the MSByte is sent 
	cmd[i]=(unsigned char)(temp&0x00FF);  //before the LSByte
	printf("crcLSB: %02x  ",cmd[i]);
	printf("crcMSB: %02x  ",cmd[i+1]);


	write_rs485BYTE(cmd,i+2, returndata, &j); /*i is the  number of input bytes to send. 
add two to this for the CRC bytes
*/
	printf("\nReturn(hex): ");

	for (i=0;i<j;i++){
		printf("%02x  ",returndata[i]);
	}
	// validate data

	printf("CRC valid?(0 yes) %d\n",validateRTU(returndata,j));



printf("\n");
}
  return 0 ;
}

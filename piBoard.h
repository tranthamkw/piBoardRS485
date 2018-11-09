/*

KENBOARD electrical layer for wiring Pi interface to RaspPi GPIO. See schematics

MODBUS RTU LAYER::
	uses Modbus 16 communcation protocal .  Modbus specifies how data is organized
	and can be transmitted over different types of electrical standards (e.g. rs485, rs232,...)
	Useful resource:
	http://www.lammertbies.nl/comm/info/modbus.html
	8 bits
	No parity
	1 stop bit(I think)
	9600 baud
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mcp3004.h>
#include <wiringPi.h>
#include <wiringSerial.h>

int initializeBoard();
//wiring pi setup , initialyze global variables
int closeBoard();

int getADC(unsigned short chan, unsigned int * returndata);
// uses MCP3008 ADC conected to SPI channel 0.
void delay_uS(int x);

/*
 RS485 I/O
*/
void write_rs485BYTE(unsigned char* cmd, unsigned int numchar, unsigned char* pszEcho, unsigned int* sizeEcho);
// uses RaspiUART and one GPIO pin to control serial IO through RS485
unsigned short modRTU_CRC(unsigned char* buff,unsigned short len);
// calculates Modbus16 CRC. Returns CRC word. This gets seperated into two bytes
int validateRTU(unsigned char* buff, unsigned short len);
//checks a byte buffer . Assumes that the last two bytes are Modbus16 CRC
int write_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int writedata);
//writes a message to machine at address, and to register reg.
// returns 0 if no errors
// returns -1 for general errors, such as CRC check error or time-out no response(100ms)
// returns machine specific error code if the machine responds
int read_Modbus_RTU(unsigned short address, unsigned short reg, unsigned int* cnReturnData);
//read from a register for a machine at address. 
//same errors are returned.
//if an error is returned then cnReturnData is junk

#include "ModBusRTU.h"
#include <pic.h>

char RS485_Init()
{
	//see page 133. Required for asynchronous
	SPEN = 1;
	TRISB5 = 1;//RX pin
	TRISB7 = 1;//TX pin
	TRISA2=0;// OUTPUT read/write control bit for max-485 module
	ANSELH = 0x00; // analog inputs 8,9,10,&11 disabled
// turns off analog ANS11. Page 70. Required for UART RX to work properly
	SYNC=0;
	BRGH=1;
	BRG16=0;
	SPBRGH = 0;
    SPBRG = 51; //9600 bps
    TXSTA = 0b10101100; 
    RCSTA = 0b10010000; 
    BAUDCTL = 0; //8-bit, Auto Baud disabled
	TXEN=1;
//	RCIE=1;  // interupts?
	RA2=0; //RS485 listen

	return 0;
}


unsigned short modRTU_CRC(char* buff, int len){
//calculates CRC for Modbus specs
	unsigned short crc=0xFFFF;
	int pos;
	int bits;

	for(pos=0;pos<len;pos++){
		crc^=(unsigned short) buff[pos];
		for(bits=8;bits!=0;bits--){
			if((crc&0x0001)!=0){
				crc>>=1;
				crc^=0xA001;
			}else{
				crc>>=1;
			}
		}
	}
return crc;
}

int validateRTU(char* buff, int len){
	/* len is the full length of the buffer.  The last two elements in the array
	are assumed to be  CRC bytes.
		*/
	int j;
	unsigned short temp;
	j=-1;
	if(len>3){
		temp=((buff[len-1]<<8)|(buff[len-2]));
		if(temp==modRTU_CRC(buff,len-2)) j=0; // valid.yes.
	}
return j;
}

int  RS485_Read_Data(unsigned char *buffer, unsigned char max){
unsigned short i,k;
char temp;
i=0;
while (RCIF) {
while (RCIF) {
while (RCIF) {
	 if (i<max){
		buffer[i] = RCREG;
		i++;
		} else temp = RCREG;
		}
	for (k=0;k<100;k++);  // small delay if there is still incoming.  This is FOSC dependent
}
	for (k=0;k<100;k++);  // small delay if there is still incoming.  This is FOSC dependent. 86. Changed to 100 2018-4-22
}
buffer[i] = 0;
return i;
}

void RS485_Write_Data(char *text, int num){
  	int i;
	unsigned short temp;
	char crc[2];
	temp=modRTU_CRC(text,num);
	crc[1]=(temp&0xFF00)>>8;
	crc[0]=(temp&0x00FF);

	RA2=1;// setRS485 to write
    for(i=0;i<num;i++){
			while (!TRMT); // wait until empty
			if (TRMT){  // transmit register empty
			TXREG = text[i];
			}
	}
	for(i=0;i<2;i++){
			while (!TRMT); // wait until empty
			if (TRMT){  // transmit register empty
			TXREG = crc[i];
			}
	}

	while (!TRMT);//wait until data is completely shifted out.
	for (i=0;i<10;i++); // minor delay
	RA2=0;
		// set RS485 to listen
}

void RS232_Write_Data(char *text, int num){
  	int i;
	
    for(i=0;i<num;i++){
			while (!TRMT); // wait until empty
			if (TRMT){  // transmit register empty
			TXREG = text[i];
			}
	}
    


	while (!TRMT);//wait until data is completely shifted out.
	for (i=0;i<10;i++); // minor delay

}

char RS485_Data_Ready()
{
	return RCIF;
}

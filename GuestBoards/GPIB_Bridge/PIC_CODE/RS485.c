#include "RS485.h"

#define RS485CONTROL PORTEbits.RE0
#define LISTEN 0
#define WRITE 1

void RS485_Init(void){
   // EUSART1_Initialize();
        // ABDOVF no_overflow; CKTXP async_noninverted_sync_fallingedge; BRG16 16bit_generator; WUE disabled; ABDEN disabled; DTRXP not_inverted; 
    BAUDCON1 = 0x08;
    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RCSTA1 = 0x90;
    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave_mode; 
    TXSTA1 = 0x24;
    // Baud Rate = 9600; 
    SPBRG1 = 0xA0;
    // Baud Rate = 9600; 
    SPBRGH1 = 0x01;
   
    
    RS485CONTROL=LISTEN;
   
 
}
void resetRS485(void){
    
    RCSTA1bits.CREN=0;
    RCSTA1bits.CREN=1;
    
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

char RS485_Data_Ready(void)
{
	return PIR1bits.RC1IF;//RC1IF;
}

int  RS485_Read_Data(char *buffer, char max){
    
    /* 9600 baud = 9600 bits per second. or 960, eight bit bytes (+1 start and +1 end bit =10bits) per second
     or 1.042 mS for every byte. 
     for (k=0;k<320;k++) is about 1 ms with at 16MHz FOSC
     we need to wait at least this long between bytes to ensure there isnt more bytes in the stream. */
unsigned short i,k;
char temp;
i=0;

while (RC1IF) {
while (RC1IF) {
while (RC1IF) {
    
	 if (i<max){
		buffer[i] = RCREG1;
		i++;
		} else temp = RCREG1;
		}
	for (k=0;k<250;k++);  // small delay if there is still incoming.  This is FOSC dependent
}
	for (k=0;k<200;k++);  // small delay if there is still incoming.  This is FOSC dependent. 86. Changed to 100 2018-4-22
}
buffer[i] = 0;

resetRS485();

return i;
}

void RS485_Write_Data(char *text, char num){
  	char i;
	unsigned short temp;
	char crc[2];
	temp=modRTU_CRC(text,num);
	crc[1]=(temp&0xFF00)>>8;
	crc[0]=(temp&0x00FF);
 
    RS485CONTROL=WRITE;
	
    for(i=0;i<num;i++){
			while (!TXSTA1bits.TRMT); // wait until empty
			if (TXSTA1bits.TRMT){  // transmit register empty
			TXREG1 = text[i];
			}
	}
	for(i=0;i<2;i++){
			while (!TXSTA1bits.TRMT); // wait until empty
			if (TXSTA1bits.TRMT){  // transmit register empty
			TXREG1 = crc[i];
			}
	}

	while (!TXSTA1bits.TRMT); //wait until data is completely shifted out.
	for (i=0;i<10;i++)
        ;//// minor delay
    
	 RS485CONTROL=LISTEN;
		// set RS485 to listen
     resetRS485();
}

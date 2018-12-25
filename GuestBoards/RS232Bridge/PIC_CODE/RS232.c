#include "RS232.h"


void RS232_Init(void){
   
    // ABDOVF no_overflow; CKTXP async_noninverted_sync_fallingedge; BRG16 16bit_generator; WUE disabled; ABDEN disabled; DTRXP not_inverted; 
    BAUDCON2 = 0x08;
    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RCSTA2 = 0x90;
    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave_mode; 
    TXSTA2 = 0x24;
    // 
    SPBRG2 = 0xA0;
    // 
    SPBRGH2 = 0x01;
    
}

void resetRS232(void){
    
    RCSTA2bits.CREN=0;
    RCSTA2bits.CREN=1;
    
}

char RS232_Data_Ready(void)
{
	return RC2IF;
            //PIR1bits.RC2IF;//RC2IF;
}

int  RS232_Read_Data(char *buffer, char max){
    
    /* 9600 baud = 9600 bits per second. or 960, eight bit bytes (+1 start and +1 end bit =10bits) per second
     or 1.042 mS for every byte. 
     for (k=0;k<320;k++) is about 1 ms with at 16MHz FOSC
     we need to wait at least this long between bytes to ensure there isnt more bytes in the stream. */
unsigned short i,k;
char temp;
i=0;

while (RC2IF) {
while (RC2IF) {
while (RC2IF) {
    
	 if (i<max){
		buffer[i] = RCREG2;
		i++;
		} else temp = RCREG2;
		}
	for (k=0;k<250;k++);  // small delay if there is still incoming.  This is FOSC dependent
}
	for (k=0;k<250;k++);  // small delay if there is still incoming.  This is FOSC dependent. 86. Changed to 100 2018-4-22
}
buffer[i] = 0;

resetRS232();
return i;
}

void RS232_Write_Data(char *text, char num){
  	int i;
	
    for(i=0;i<num;i++){
			while (!TXSTA2bits.TRMT); // wait until empty
			if (TXSTA2bits.TRMT){  // transmit register empty
			TXREG2 = text[i];
			}
	}
    
	while (!TXSTA2bits.TRMT);//wait until data is completely shifted out.
	for (i=0;i<10;i++)
        ; // minor delay

    resetRS232();
}

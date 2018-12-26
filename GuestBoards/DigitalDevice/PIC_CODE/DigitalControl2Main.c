#include <pic.h>
#include <string.h>
#include "ModBusRTU.h"
#include "SSD1306.h"


/*

Microcontroller listens to RS485 commands and sets a servo to one of 8 positions. 



Set Servo to one of eight positions:
Write 
<RS485Address> 06 0A 0A 00 <pos> <CRCh> <CRCl>
with <pos> in the range 0 to 8

Responds with echo, or shows error in byte[1]

*/

__CONFIG(INTIO & WDTDIS & PWRTEN & MCLRDIS & UNPROTECT & BORDIS &
IESODIS & FCMDIS);

#define BASEREG 0x0A0A
#define FNREG 0x00F0

#define MYFUNCTION "DigitalOut"
unsigned char address=0xA0;


void updateDisplayStatus(void){
	char temp[16],outText[24];
	unsigned int k;
	clearDisplaySSD1306();
	strcpy(outText,"FUNCTION: ");
	strcat(outText,MYFUNCTION);
	k=strlen(outText);
	writeCharSSD1306(0,0,outText,k,0);

	strcpy(outText,"RS485Address: 0x");
	itoa(address,&temp,16,2);
	strcat(outText,temp);
	k=strlen(outText);
	writeCharSSD1306(1,0,outText,k,0);

	strcpy(outText,"RA4 ");
	itoa(RA4,temp,2,1);
	strcat(outText,temp);
	k=strlen(outText);
	writeCharSSD1306(3,0,outText,k,0);

	strcpy(outText,"RA5 ");
	itoa(RA5,temp,2,1);
	strcat(outText,temp);
	k=strlen(outText);
	writeCharSSD1306(3,10,outText,k,0);

}

static void interrupt isr(void)
{
 
}

void main (void) {	
 	int i,k;
	int reg;
	int strlength;
	char error;
	char data[24];
	char fnString[16];
	unsigned short outdata;
	
	PORTA =0;    //clears port A
	TRISA=0;
	TRISC = 0;    
	PORTC = 0;    //clears port C
	CM1CON0 = 7;    //turn off comparators
	CM2CON0 = 7;
	CM2CON1 = 7;
	ANSEL = 0;    //turns off ADC
	OSCCON=0x70;         // Select 8 MHz internal clock
	address=EEPROM_READ(0x00);
	RS485_Init();


 	initSSD1306();
	updateDisplayStatus();
	
	strlength=0;
	error=0;

  do  {

	if (RS485_Data_Ready()==1){
			strlength = RS485_Read_Data(data,24);
			CREN=0;	
			if (validateRTU(data,strlength) ==0){
					// RTU CRC valid
					if (!RA3) {	// if button pressed, remember this address as our own.
						address = data[0];
						EEPROM_WRITE(0x00,address);
					}
					delay(100);
					if (data[0]==address){ //address byte
							// message intended for us
						switch (data[1]){  // command code byte
				
							case 0x06: // write a register
								reg = (data[2]<<8) | (data[3]);
									switch (reg){
									case BASEREG +16:
										RA4=(data[5] & 0x01);
										RA5=((data[5] & 0x02)>>1);
										strlength=6;
										RS485_Write_Data(data,strlength);
										error = 0;
										break;
									default:
										// invalid register
										error = 0x02;
										break;
										}
								break; // write to a register
							case 0x03:  // read a register
								reg = (data[2]<<8) | (data[3]);
								switch (reg) {
										case BASEREG+16:
										// formulate a response. 
											data[2]=2;  //number of bytes to follow of data
											data[3]=0;
											data[4]=(RA5<<1) | RA4;
											strlength=5;
											RS485_Write_Data(data,strlength);
											error =0;
											break;
										case FNREG:
											strcpy(fnString,MYFUNCTION);
											strlength = strlen(fnString);
										//data[0]=<RS485Address>; //data[1]=03;
											data[2] = (unsigned char)strlength;// number of bytes to follow
											for (i=0;i<strlength;i++){
												data[3+i]=fnString[i];
											}
											strlength+=3;
											RS485_Write_Data(data,strlength);
											error =0;
											break;
										default:
										error = 0x02; // invalid register
										break;
								}		
							break;// read a register
						
							default:
								// unknown command byte
								error = 0x08;
							break;
						}//end switch
						if (error) {
								data[1]=data[1] | 0x80;
								data[2]= (error & 0xFF00)>>8;
								data[3]=(error & 0x00FF);
								strlength=4;
								RS485_Write_Data(data,strlength);
						}
						updateDisplayStatus();
					}/// message intended for another address. ignore message.
			}else {
					//invalid RTU  just ignore the message
			}
		
			CREN=1; // this sequence resets the receive register. The act of writing tends to load a single bit 
			CREN=0;
			CREN=1;
		}// end data ready
 
}while(1);
}
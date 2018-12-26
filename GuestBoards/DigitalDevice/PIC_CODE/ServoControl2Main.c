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
#define MAX_COUNT 200
#define MYFUNCTION "DUAL SERVO"
unsigned char address=0xA0;

unsigned char time_count=0;  
unsigned char servo1_count=10;  //counts number of 0.1mS.  Minimum is 10. Maximum is 20
unsigned char servo2_count=15;

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

	strcpy(outText,"SERVO 1: ");
	itoa(servo1_count-10,temp,10,2);
	strcat(outText,temp);
	k=strlen(outText);
	writeCharSSD1306(2,0,outText,k,0);


	strcpy(outText,"SERVO 2: ");
	itoa(servo2_count-10,temp,10,2);
	strcat(outText,temp);
	k=strlen(outText);
	writeCharSSD1306(3,0,outText,k,0);

}


static void interrupt isr(void)
{
  if(T0IF) {	            // TIMER0 Interrupt Flag
	time_count++; // counts the number of times we go through this interupt. This interrupt is called every 0.1 mS
    
    /* MAX_VALUE=200  */
    if (time_count == MAX_COUNT) { //reset everything and turn on 
		RA5=1;
		RA4=1;
      	time_count=0;
        }
    if (time_count == servo1_count) {
		RA5=0;
      }
	if (time_count == servo2_count) {
		RA4=0;
      }
    TMR0 = 180;             // Initial Value for 0.1ms Interrupt
    T0IF = 0;		    // Clear TIMER0 interrupt flag
  }
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

   OPTION = 0b00000000; // 1:2 Prescaller
  TMR0=180;            // Interupt every 0.1 ms
  T0IE = 1;		       // Enable interrupt on TMR0 overflow
  GIE = 1;		       // Global interrupt enable
	strlength=0;
	error=0;

	RS485_Init();
 	initSSD1306();
	updateDisplayStatus();


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
					delay(300);
					if (data[0]==address){ //address byte
							// message intended for us
						switch (data[1]){  // command code byte
				
							case 0x06: // write a register
								reg = (data[2]<<8) | (data[3]);
									switch (reg){
									case BASEREG:
										outdata =(data[4]<<8)| data[5];  // assign new data to varible ("register")
										if (outdata<9){
											servo1_count=outdata + 10;
											// formulate a response echo back command
											strlength=6;
											RS485_Write_Data(data,strlength);
											error = 0;
												}else {
												error = 0x06; // out of range
												}
										break;
									case (BASEREG+1):
										outdata =(data[4]<<8)| data[5];  // assign new data to varible ("register")
										if (outdata<11){
											servo2_count=outdata + 10;
											// formulate a response echo back command
											strlength=6;
											RS485_Write_Data(data,strlength);
											error = 0;
												}else {
												error = 0x06; // out of range
												}
										break;
									case FNREG:
										if (data[4]==address){
											address=data[5];
											EEPROM_WRITE(0x00,address);
											strlength=6;
											RS485_Write_Data(data,strlength);
										} else {
										error = 0x02;
										}
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
										case BASEREG:
											outdata = (unsigned short) servo1_count;
											outdata-=10;
										// formulate a response. 
											data[2]=2;  //number of bytes to follow of data
											data[3]=(outdata&0xFF00)>>8; //MSB first
											data[4]=(outdata&0x00FF);
											strlength=5;
											RS485_Write_Data(data,strlength);
											error =0;
											break;
										case (BASEREG+1):
											outdata = (unsigned short) servo2_count;
											outdata-=10;
										// formulate a response. 
											data[2]=2;  //number of bytes to follow of data
											data[3]=(outdata&0xFF00)>>8; //MSB first
											data[4]=(outdata&0x00FF);
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
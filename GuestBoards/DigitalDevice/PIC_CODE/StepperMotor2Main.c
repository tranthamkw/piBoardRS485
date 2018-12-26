#include <pic.h>
#include <string.h>
#include "ModBusRTU.h"
#include "SSD1306.h"


#define BASEREG 0x0B0B

#define FNREG 0x00F0
#define MYFUNCTION "Step Motor"

/*

MODBUS RTU byte order for writing to a register
data[0]			data[1]		data[2]				data[3]				data[4]			data[5]
[RS485Address]  [06] 		[RegisterHighByte]	[RegisterLowByte]	[ValueHighByte]	[ValueLowByte] {CRC High] [CRC Low]


BASEREG:  read/write the number of steps and direction to move motor.
Write: Initiates a move. During the move, the number of steps is incremented down to zero
Read: Current status; i.e. number of steps remaining 
format: 
	number of pulses =(((data[4])&0b01111111)<<8)| data[5];
	direction to move = RA4=(data[4]>>7);
Example
D0 06 0F 0F 80 C1 [crc] [crc]
if this device's address is D0, then 0xC1 steps are excecuted in the '1' direction

BASEREG+1: command/macro to execute. such as automatic homing. 

RA4 = direction, output
RA5 = clock pulses, output
RC4 = homing input

*/


__CONFIG(INTIO & WDTDIS & PWRTEN & MCLRDIS & UNPROTECT & BORDIS &
IESODIS & FCMDIS);

// Servo definition and variables

unsigned char address=0;
unsigned char timecount=0;
unsigned char timeon=4;
unsigned char timeoff=8;
unsigned int pulseCount;
unsigned int pulseTime = 100;
unsigned char findHome;
unsigned short homePulseCount=0;
unsigned int pulsePerRev=100;
unsigned char activestate=1;
unsigned char homedir=0;
char outText[24];

void updateDisplayStatus(void){
	char temp[8];
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

	strcpy(outText,"PulseT:");
	itoa(pulseTime,temp,10,2);
	strcat(outText,temp);
	k=strlen(outText);
	writeCharSSD1306(2,0,outText,k,0);

}

static void interrupt isr(void){
  if(T0IF) {	 
    if (findHome==1) {
		findHome=(RC4^activestate);
		RA4=homedir; //homing direction
	    pulseCount=1;
	
   }
	if (homePulseCount>pulsePerRev){
		// rotated a full rev without finding home.so stop
	findHome=0;
	homePulseCount=0;
	}
	if (pulseCount>0){

		if (timecount<=timeon){
			RA5=!(activestate);
			timecount++;
		}
		if ((timecount>timeon)&(timecount < (timeon+timeoff))) {
			RA5=activestate;
			timecount++;
		}
		if (timecount==(timeon+timeoff)){
			timecount=0;
			pulseCount--;
			if(findHome==1) homePulseCount++;
			RA5=activestate;
		}
	}
  	TMR0=(255-pulseTime);      
    T0IF = 0;		    // Clear TIMER0 interrupt flag
	} 
}


void main (void) {	

 	int i,k;
	int reg;
	int strlength;
	char error;
	char data[24];
 
	
	PORTA =0;    //clears port A
	TRISA=0;
	TRISC = 0xFF;    
	PORTC = 0;    //clears port C
	CM1CON0 = 7;    //turn off comparators
	CM2CON0 = 7;
	CM2CON1 = 7;
	ANSEL = 0;    //turns off ADC
	//ANSELH = 0; // turns off ADC See page 70
	//TRISA = 0b111111;    //sets port A inputs 
	
	OSCCON=0x70;         // Select 8 MHz internal clock

	address=EEPROM_READ(0x00);
	RS485_Init();
initSSD1306();
updateDisplayStatus();

  /*
TIMER 0 INTERRUPT TIMING
Fosc	8000000		Period mS
Fosc/4	2000000	0.0000005	0.0005
Prescale
Last 3 bits of OPTION
		scale	Period mS per TMR0 tick
	000	2		0.001
	001	4		0.002
	010	8		0.004
	011	16		0.008
	100	32		0.016
	101	64		0.032
	110	128		0.064
	111	256		0.128

An interrupt is generated when TMR0 counts from 0xFF to 0x00.
So, the time between interrupts is (255 - TMR0) * period per tick
Smallest usable value of TMR0 is 1. Largest is 254.
*/

  OPTION = 0b00000011; // see table above
  TMR0=(255-pulseTime);   
 
  T0IE = 1;		       // Enable interrupt on TMR0 overflow
  GIE = 1;		       // Global interrupt enable

strlength=0;
findHome = 0;
error=0;
pulseCount=0;
homePulseCount=0;
RA5=activestate;



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
			if (data[0]==address){ //address byte
							// message intended for us
				switch (data[1]){  // command code byte
				case 0x06: // write a register
				reg = (data[2]<<8) | (data[3]);
					switch (reg){
					case BASEREG:
						pulseCount =(((data[4])&0b01111111)<<8)| data[5];  // assign new data to varible ("register")
						RA4=(data[4]>>7);
						strlength=6;
						RS485_Write_Data(data,strlength);
						error = 0;
						break;
					case BASEREG+1:
						pulsePerRev =((data[4])<<8)| data[5];  // assign new data to varible ("register")
						if (pulsePerRev==0) pulsePerRev=100;
						strlength=6;
						RS485_Write_Data(data,strlength);
						error = 0;
						break;
					case BASEREG+4:
						pulseTime =((data[4])<<8)| data[5];  // assign new data to varible ("register")
						if (pulseTime<50) pulseTime=50;
						if (pulseTime>250) pulseTime=250;
						strlength=6;
						RS485_Write_Data(data,strlength);
						error = 0;
						break;
					case (BASEREG+16): // macro to find home.  data[5] contains bit definitions for which direction to move motor and what state to look for.
						findHome=(data[5]&0b00000001);
						activestate=(data[5]&0b00000010)>>1;
						homedir=(data[5]&0b00000100)>>2;
						strlength=6;
						RS485_Write_Data(data,strlength);
						error = 0;
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
					case BASEREG: // return number of pulses to go. When pulseCount=0, the motor is done
						data[2]=2;  //number of bytes to follow of data
						data[3]=(pulseCount&0xFF00)>>8; //MSB first
						data[4]=(pulseCount&0x00FF); // then LSB
						strlength=5;
						RS485_Write_Data(data,strlength);
						error =0;
						break;
					case (BASEREG+1): 
						data[2]=2;  //number of bytes to follow of data
						data[3]=(pulsePerRev&0xFF00)>>8; //MSB first
						data[4]=(pulsePerRev&0x00FF); // then LSB
						strlength=5;
						RS485_Write_Data(data,strlength);
						error =0;
						break;
					case (BASEREG+4): 
						data[2]=2;  //number of bytes to follow of data
						data[3]=(pulseTime&0xFF00)>>8; //MSB first
						data[4]=(pulseTime&0x00FF); // then LSB
						strlength=5;
						RS485_Write_Data(data,strlength);
						error =0;
						break;
					case (BASEREG+2): 
						data[2]=2;  //number of bytes to follow of data
						data[3]=0x00; //MSB first
						data[4]=!(RC4^activestate); // then LSB
						strlength=5;
						RS485_Write_Data(data,strlength);
						error =0;
						break; 
					case FNREG:
						strcpy(outText,MYFUNCTION);
						strlength = strlen(outText);
						//data[0]=<RS485Address>; //data[1]=03;
						data[2] = (unsigned char)strlength;// number of bytes to follow
						for (i=0;i<strlength;i++){
							data[3+i]=outText[i];
						}
						strlength+=3;
						RS485_Write_Data(data,strlength);
						error =0;
						break;
					default:
						error = 0x02;
						break;
					}		
					break;// read a register
				default:
				// unknown command
					error = 0x08;
				break;
				}//end switch
				
			if (error) {
			data[1]= data[1] | 0x80;
			data[2]= (error & 0xFF00)>>8;
			data[3]= (error & 0x00FF);
			strlength=4;
			RS485_Write_Data(data,strlength);
				}
			updateDisplayStatus();
			}/// message intended for another address. ignore message.
			}else {
	//invalid RTU			
			}
			CREN=1; // this sequence resets the receive register. The act of writing tends to load a single bit 
			CREN=0;
			CREN=1;
		}// end data ready

}while(1);
}

#include <pic18f46k22.h>

#include "mySPI.h"

//#define WCOL SSP1CON1bits.WCOL


void initSPI(){
 
  
	SSP1STAT = 0b10000000;
			/*
			SMP		CKE		D/!A		P		S		R/!W		UA		BF
			page 158 of data sheet
			SMP- data INPUT sample phase bit. 1 at end of data, 0 at middle of data
			CKE - SPI clock edge select bit. Depends on CKP in register SSPCON
			BF - buffer full status bit
			*/
    SSP1CON1 = 0b00110000;
			/*
			WCOL	SSPOV	SSPEN	CKP		 SSPM<0:3>
			7:WCOL - write collision detect bit
			6:SSPOV - Receive overflow indicator bit
			5:SSPEN - serial port enable. 1 endables and configures SCK, SDO, and SDI as serial port points. 0 disables and configures pins as general I/O
			4:CKP - clock polarity
			3-0: SSPM port mode select.
				0000 = SPI Master mode, clock = FOSC/4
				0001 = SPI Master mode, clock = FOSC/16
				0010 = SPI Master mode, clock = FOSC/64
				0011 = SPI Master mode, clock = TMR2 output/2
				0100 = SPI Slave mode, clock = SCK pin. SS pin control enabled.
				0101 = SPI Slave mode, clock = SCK pin. SS pin control disabled. SS can be used as I/O pin.
				0110 = I2C Slave mode, 7-bit address
				0111 = I2C Slave mode, 10-bit address
				1000 = Reserved
				1001 = Load SSPMSK register at SSPADD SFR address(2)
				1010 = Reserved
				1011 = I2C Firmware Controlled Master mode (slave IDLE)
				1100 = Reserved
				1101 = Reserved
				1110 = I2C Slave mode, 7-bit address with Start and Stop bit interrupts enabled
				1111 = I2C Slave mode, 10-bit address with Start and Stop bit interrupts enabled
			*/
}

unsigned char writeSPI(char data_out){
    unsigned char TempVar;

    TempVar = SSP1BUF;           // Clears BF
    SSP1IF = 0;         // Clear interrupt flag
    SSP1CON1 = 0b00110000;
    //WCOL = 0;            //Clear any previous write collision
    SSP1BUF = data_out;           // write byte to SSPBUF register
   
    if (SSP1CON1 & 0x80)        // test if write collision occurred WCOL & 0x80
      TempVar=1;              
    else {
        while(!SSP1IF);  // wait until bus cycle complete
	     TempVar=0;
		 }

	return(TempVar);
}
 	
#include "spi.h"

void initSPI(){
 GIE = 1;	
  // Master Mode
	ANSELH = 0x00; // analog inputs 8,9,10,&11 disabled

/*
	RB4=AN10=SDI
	RB5=AN11=RX/DT
	RB6=SCK/SCL
	RB7=TX/CK
	RC7=AN9=SDO
	RC6=AN8=!SS
*/

 //TRISB=0xFF; // all digital 'in'.  set serial communication 

  TRISC7 = 0;  // SDO/SPIDAT  port C7 an output
  TRISC6 = 0; // CS bit an output
  TRISB6 = 0; //SCK/SPICLK serial clock an output

	//TODO SDI serial data in. for devices that return data
  
	SSPSTAT = 0b10000000;
			/*
			SMP		CKE		D/!A		P		S		R/!W		UA		BF
			page 158 of data sheet
			SMP- data INPUT sample phase bit. 1 at end of data, 0 at middle of data
			CKE - SPI clock edge select bit. Depends on CKP in register SSPCON
			BF - buffer full status bit
			*/
    SSPCON = 0b00110000;
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

    TempVar = SSPBUF;           // Clears BF
    SSPIF = 0;         // Clear interrupt flag
    WCOL = 0;            //Clear any previous write collision
    SSPBUF = data_out;           // write byte to SSPBUF register
    if ( WCOL & 0x80 )        // test if write collision occurred
      TempVar=1;              
    else {
        while(!SSPIF);  // wait until bus cycle complete
	     TempVar=0;
		 }

	return(TempVar);
}
 	
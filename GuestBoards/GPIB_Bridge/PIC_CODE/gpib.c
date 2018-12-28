
// Based on code from Raphael Kuhnen <raphael.kuhnen@fmf.uni-freiburg.de> and others.
//
// Credits: Ruud Baltissen - http://ftp.osuosl.org/pub/nslu2/sources/xmame-0.100.tar.bz2
//
// PIC Pin addignments:
//
// RD0 - REN
// RD1 - EOI
// RD2 - DAV
// RD3 - NRFD
// RD4 - NDAC
// RD5 - IFC
// RD6 - SRQ
// RD7 - ATN
//
// RA0 - D1
// RA1 - D2
// RA2 - D3
// RA3 - D4
// RA4 - D5
// RA5 - D6
// RA6 - D7
// RA7 - D8
//
//
// GPIB Connector pinout
//
//Pin  	Name 	Signal Description 			Pin Name 	Signal Description 
//1 	DIO1 	Data Input/Output Bit 1 	13 	DIO5 	Data Input/Output Bit 5 
//2 	DIO2 	Data Input/Output Bit 2 	14 	DIO6 	Data Input/Output Bit 6 
//3 	DIO3 	Data Input/Output Bit 3 	15 	DIO7 	Data Input/Output Bit 7 
//4 	DIO4 	Data Input/Output Bit 4 	16 	DIO8 	Data Input/Output Bit 8 
//5 	EIO 	End-Or-Identify 			17 	REN 	Remote Enable 
//6 	DAV 	Data Valid 					18 	Shield 	Ground (DAV) 
//7 	NRFD 	Not Ready For Data 			19 	Shield 	Ground (NRFD) 
//8 	NDAC 	Not Data Accepted 			20 	Shield 	Ground (NDAC) 
//9 	IFC 	Interface Clear 			21 	Shield 	Ground (IFC) 
//10 	SRQ 	Service Request 			22 	Shield 	Ground (SRQ) 
//11 	ATN 	Attention 					23 	Shield 	Ground (ATN) 
//12 	Shield 	Chassis Ground 				24 	Single GND 	Single Ground


#include <pic18f46k22.h>

#include "gpib.h"

#define GPIB_REN PORTDbits.RD0
#define GPIB_EOI PORTDbits.RD1
#define GPIB_DAV PORTDbits.RD2
#define GPIB_NRFD PORTDbits.RD3
#define GPIB_NDAC PORTDbits.RD4
#define GPIB_IFC PORTDbits.RD5
#define GPIB_SRQ PORTDbits.RD6
#define GPIB_ATN PORTDbits.RD7


unsigned char eoiflag;
unsigned char atnflag;
unsigned char gpib_buff[GPIB_MAX_RX_LEN];
unsigned char gpib_ptr;
char gpib_tx_buff[GPIB_MAX_TX_LEN];
char gpib_tx_ptr;
char gpib_cmd_buff[GPIB_MAX_CMD_LEN];
unsigned char gpib_cmd_ptr;
unsigned char listening;
unsigned char in_command;
//

unsigned char i,j;
unsigned long count, timeout;
char string[GPIB_MAX_TX_LEN];
unsigned char string_offset;
unsigned char rx_state;				// Rx state machine state
unsigned char tx_state;				// Tx state machine state


void gpib_init()
{
	
    PORTD=0xFF;  // all false
    TRISA = 0xFF; //inputs
    TRISD = 0xFF; // inputs
    TRISDbits.TRISD0=0; //REN output
    GPIB_REN=0; // assert REN
    
	//
 	// Zero the GPIB buffers and pointers
	//
	gpib_ptr = 0;
	gpib_tx_ptr = 0;
	gpib_cmd_ptr = 0;
	listening = 0;
	in_command = 0;
	i=0;
	while(i<GPIB_MAX_RX_LEN)
	{
		gpib_buff[i] = 0;
		i++;
	}
	i=0;
	while(i<GPIB_MAX_TX_LEN)
	{
		gpib_tx_buff[i] = 0;
		i++;
	}
}

/*
-----------------------------------------------------------------------------------------------------------------------
// Send and receive routines are written as finite-state-machines. Please refer to the timing diagram on page 14 of
// "A tutorial Description of the Hewlett-Packard Interface Bus" (http://bitsavers.org/pdf/hp/hpib/TutorialDescrOfHPIB.pdf)
//-----------------------------------------------------------------------------------------------------------------------
//
// sendgpib() - Send GPIB string in gpib_tx_buff[]
//
// Buffer layout:
// |--0--|--1--|--2--|--3--|--4--|--5--|-....
//   cmd   len   data data  data ...
//
// If cmd=='2' then it is a command and ATN is set true (low)
// len is number of bytes to send.
// EOI is asserted if the last character is LF (0x10)
*/

void sendgpib(void) {
	tx_state = GPIB_TX_START;
	j=2;
	
    
	TRISA = (TRISA | 0b11011111);			// Do not touch REN.
	string_offset=0;

	if(gpib_tx_buff[0] == '2') 				// Is it a command?
	{
        TRISDbits.TRISD7=0; // output
		GPIB_ATN = 0; // ATN = 0
		
	} 
    
    
	while(tx_state != GPIB_TX_DONE){
        
		switch(tx_state){
			case GPIB_TX_START:
				tx_state = GPIB_TX_CHECK;
				break;
			case GPIB_TX_CHECK:
				
					tx_state = GPIB_TX_PUT_DATA;
				break;
			case GPIB_TX_PUT_DATA:
				
				if (j == gpib_tx_buff[1]-1)	{// last character
                    TRISDbits.TRISD1 = 0;// EOI output
                    GPIB_EOI = 0;
                 }
				
				PORTA = ~gpib_tx_buff[j];					// Put data on bus
				++j;
				TRISA=0x00; 										// Make data lines output
																	//		A GPIB Talker, transfers data bytes to GPIB Listeners, it drives
																	//		the data byte on the GPIB DIO[8?1] signals. After waiting for a certain delay (known as
																	//		the T1 delay), the Talker asserts DAV to indicate to the Listeners that the data byte
																	//		has settled on the DIO[8?1] signals. A delay of 2us should satisfy most implementations.
																	//		http://www.ni.com/pdf/manuals/370875a.pdf
				count = 20;
				while(count){count--;} 								// wait to settle line
				tx_state = GPIB_TX_WAIT_FOR_NRFD;
				count = TIMEOUT;
				break;
			case GPIB_TX_WAIT_FOR_NRFD:								// Wait for NRFD high
                TRISDbits.TRISD2=1; // set DAV input
				if (GPIB_DAV == 1 || count-- = 0) tx_state = GPIB_TX_SET_DAV_LOW;
				break;
			case GPIB_TX_SET_DAV_LOW:
				//		DAV = 0;
				TRISDbits.TRISD2 = 0;
                GPIB_DAV=0;
				tx_state = GPIB_TX_WAIT_FOR_NDAC;
				count = TIMEOUT;
				break;
			case GPIB_TX_WAIT_FOR_NDAC:
                TRISDbits.TRISD4 = 1;
				if (GPIB_NDAC == 1 || count--<=0) tx_state = GPIB_TX_SET_DAV_HIGH;
				break;
                
			case GPIB_TX_SET_DAV_HIGH:
				TRISA=0xFF; 										// data lines tristate
                TRISDbits.TRISD1= 0;
				GPIB_EOI = 1; 								// EOI false
				//DAV = 1;
				TRISDbits.TRISD2=0;								// DAV high
                GPIB_DAV=1;
				//
				// More data?
				//
				if (j < gpib_tx_buff[1]) {
                    tx_state = GPIB_TX_CHECK;
                } else {
                    tx_state = GPIB_TX_DONE;
                }
				break;
			case GPIB_TX_ERROR:

                
				tx_state = GPIB_TX_DONE;
				break;
		}
	}
    
	// All sent
	if(gpib_tx_buff[0] == '2') {TRISAbits.TRISA3 = 1;} // ATN = 1
	TRISCbits.TRISC0 = 1;
	TRISC = 0xff;													// DAV, NRFD etc threestate (input)
	TRISB = 0xff;													// Data threestate (input)
	TRISA = (TRISA | 0b11011111);									// Do not touch REN.

#if defined(SDEBUG)
	sprintf(string,(const far rom char*)"\r\n");
	sio_puts((unsigned char*)string);
#endif
	string_offset = 0;

}

//
// Receive a string of maximum GPIB_MAX_LEN chars from GPIB. Routine will loop forever and only exit when it has seen an LF, EOI or if the 
// buffer is full. 
//
// returns the actual number of bytes received or 255 if timeout occurred, hence you cannot return more than 254 bytes, but then again
// will you have problems allocating a buffer that large anyway...
//
int readgpib(void)
{
unsigned char timeoutflag=0;
	timeout = 2000;
	gpib_ptr=0;
	LATA = (LATA & 0b00100000);			// Do not touch REN.
	LATC = 0x00;
	TRISC = 0xf9;
	TRISB = 0xff;
	TRISA = (TRISA | 0b11011111);			// Do not touch REN.
	rx_state = GPIB_RX_START;
	memset(gpib_buff,0,GPIB_MAX_RX_LEN); // Init buffer to a known state

	while(rx_state != GPIB_RX_DONE)
	{
		//sprintf(string,(const far rom char*)"\r\nRX State:%d %d %X %X %ld ", rx_state,gpib_ptr ,PORTB, PORTC, timeout);
		//sio_puts((unsigned char*)string);
		//
		//CDCTxService(); moved 20130903
		//
		//
		// if((PORTB ==245 && gpib_ptr>0)||  gpib_ptr>=GPIB_MAX_LEN || !USBHandleBusy(CDCDataOutHandle)) // 245 is LF inverted 
		if(gpib_ptr>=GPIB_MAX_RX_LEN) // || !USBHandleBusy(CDCDataOutHandle))
		{										// Break out if Buffer full or USB rx
			rx_state = GPIB_RX_FINISH;
		}
		if (gpib_ptr>0)							// If we have data in buffer, check is last one was LF
		{
			if(gpib_buff[gpib_ptr-1] == 0x0a && !Binary) rx_state = GPIB_RX_FINISH;
		} 
		//
		// Parallel poll? 
		//
		// According to the SS 80 manual, the device with ID0 should pull data like DIO8 low to indicate completion
		//
		if(PORTAbits.RA4 == 0 && PORTAbits.RA3 == 0)
		{
			LATB = 0x00;								// Put status bit on bus
			TRISB=0x7f; 								// Just bit 7 (DIO8) active for now
			sprintf(string,(const far rom char*)"%S","<PP>");
			sio_puts((unsigned char*)string);
			TRISB=0xff; 

		}
		switch(rx_state)
		{
			case GPIB_RX_START:
				//		NRFD = 0;
				TRISCbits.TRISC1 = 0;			// Set both NRFD and NDAC low
				//NDAC = 0;
				TRISCbits.TRISC2 = 0;
				rx_state = GPIB_RX_ACCEPT;
				break;
	
			case GPIB_RX_ACCEPT:

				//		NRFD = 1;
				TRISCbits.TRISC1 = 1;			// Set NRFD high
				timeout = 2000;
				rx_state = GPIB_RX_WAIT_DAV;
				//
				CDCTxService();
				//
				break;
	
			case GPIB_RX_WAIT_DAV:				// Wait here for DAV to go low

				if (PORTCbits.RC0 == 0) rx_state = GPIB_RX_DAV_LOW;
				--timeout;
				if(timeout <=0) rx_state = GPIB_RX_DAV_TIMEOUT;
				break;
	
			case GPIB_RX_DAV_LOW:
				//		NRFD = 1;
				timeout = 2000;
				TRISCbits.TRISC1 = 0;			// Set NRFD low

				gpib_buff[gpib_ptr] = ~PORTB;	// Accept databyte
				//
				eoiflag=PORTAbits.RA4;
				atnflag=PORTAbits.RA3;
				if(atnflag == 0) in_command =1;
				//
				if(!atnflag && ~PORTB == 0x20)	// Addressed 0 to listen?
				{
					listening = 1;
				}

				if(!atnflag && ~PORTB == 0x3f)	// Unlisten
				{
					listening = 0;
				}
				//if (eoiflag == 0) in_command = 0;

				if(in_command==1)
				{
					gpib_cmd_buff[gpib_cmd_ptr] = ~PORTB;	// Accept command databyte
					if(gpib_cmd_ptr < GPIB_MAX_CMD_LEN-1) gpib_cmd_ptr++;	// TODO: Report overflow
				}
				//
				gpib_ptr++;
				//
				count = 10;
				//while (count)
				//{
				//	count--;
				//}
				//NDAC = 1;
				TRISCbits.TRISC2 = 1;			// Set NDAC high
				rx_state = GPIB_RX_WAIT_DAV_HIGH;
				break;
	
			case GPIB_RX_WAIT_DAV_HIGH:			// Wait here for DAV to go high
	
				if (PORTCbits.RC0 == 1) rx_state = GPIB_RX_DAV_HIGH;
				break;
	
			case GPIB_RX_DAV_HIGH:
				//NDAC = 0;
				TRISCbits.TRISC2 = 0;			// Set NDAC low
				if (eoiflag==0)			// seen an EOI? EOI is set on the last byte of data message
				{
					rx_state = GPIB_RX_EOI;
					count = 1000;						// Prime timeout counter
#if defined(SDEBUG)
					//sprintf(string,(const far rom char*)"<EOI>");
					//sio_puts((unsigned char*)string);
#endif
					if(gpib_flags & FLAGS_EOT_ENABLE)
					{
						gpib_buff[gpib_ptr++] = gpib_eot_char;
					}
					break;
				}
				rx_state = GPIB_RX_ACCEPT;		// and back to the top
				break;
	
			case GPIB_RX_EOI:					// Seen an EOI, ack and wait here until it goes away or timeout
				//NDAC = 1;
				TRISCbits.TRISC2 = 1;			// Set NDAC high
				if (PORTAbits.RA4 == 1) rx_state = GPIB_RX_FINISH;
				count--;
				if (count <=0) rx_state = GPIB_RX_FINISH;
				//
				in_command = 0;
				break;

			case GPIB_RX_DAV_TIMEOUT:
				//sprintf(string,(const far rom char*)"|");
				//sio_puts((unsigned char*)string);
				timeoutflag=1;
				in_command=0;
			case GPIB_RX_FINISH:
				//		NRFD = 0;
				TRISCbits.TRISC1 = 0;			// Set both NRFD and NDAC low
				//NDAC = 0;
				TRISCbits.TRISC2 = 0;
				rx_state = GPIB_RX_DONE;
				break;
		}
	} // while
	//
#if defined(SDEBUG)
	if(gpib_ptr > 0)
	{
		//gpib_buff[gpib_ptr] = 0;
		sprintf(string,(const far rom char*)"\r\nRX: %d timeout %d cmd %d incmd %d",gpib_ptr, timeoutflag, gpib_cmd_ptr, in_command);
		sio_puts((unsigned char*)string);
	}
#endif

	if(atnflag == 0 && gpib_buff[gpib_ptr-1] == 0x60 && gpib_buff[gpib_ptr-2] == 0x5f)	// GPIB Ident sequence?
	{
		gpib_flags = gpib_flags | FLAGS_EOI;
		gpib_tx_buff[0] = ' ';
		gpib_tx_buff[1] = 4;				// 2+2 bytes
		gpib_tx_buff[2] = 0x01;				// Identify ourselves as a HP 9121 drive
		gpib_tx_buff[3] = 0x04;				// $0104
		sendgpib();
		gpib_flags = gpib_flags & ~FLAGS_EOI;
		sprintf(string,(const far rom char*)"<ident>");
		sio_puts((unsigned char*)string);
		gpib_ptr = 0;
		gpib_cmd_ptr = 0;
	}
	if(timeoutflag)
	{
		return 255;
	}
	else
	{
		return gpib_ptr;
	}
}

//
// Interface clear
//
void gpib_ifc(void)
{
	TRISAbits.TRISA1 = 0;
	PORTAbits.RA1 = 0;
	//
	// Hold IFC low for >100us
	count = TIMEOUT;
	while (count)
	{
		count--;
	}
	TRISAbits.TRISA1 = 1;
	PORTAbits.RA1 = 1;
	count = TIMEOUT;
	while (count)
	{
		count--;
	}
}

//
// Assert REN to put instrument in remote mode
// state = true -> assert REN, state = false -> deasert REN
//
void gpib_ren(unsigned char state)
{
	if(state == true)
	{
		TRISAbits.TRISA5 = 0;		// Assert REN
		PORTAbits.RA5 = 0;
	}
	else
	{
		TRISAbits.TRISA5 = 1;		// Deassert REN
		PORTAbits.RA5 = 1;
	}
}

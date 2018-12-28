
#include <pic18f46k22.h>

#include "IEEE488.h"
/*
 pic18F46K22 PIN ASSIGNMENTS:
  
 RD0 - REN              RA0 - D1
 RD1 - EOI              RA1 - D2
 RD2 - DAV              RA2 - D3
 RD3 - NRFD             RA3 - D4
 RD4 - NDAC             RA4 - D5
 RD5 - IFC              RA5 - D6
 RD6 - SRQ              RA6 - D7
 RD7 - ATN              RA7 - D8

 GPIB Connector pinout

Pin  	Name 	Signal Description 			Pin Name 	Signal Description 
1 	DIO1 	Data Input/Output Bit 1 	13 	DIO5 	Data Input/Output Bit 5 
2 	DIO2 	Data Input/Output Bit 2 	14 	DIO6 	Data Input/Output Bit 6 
3 	DIO3 	Data Input/Output Bit 3 	15 	DIO7 	Data Input/Output Bit 7 
4 	DIO4 	Data Input/Output Bit 4 	16 	DIO8 	Data Input/Output Bit 8 
5 	EIO 	End-Or-Identify 			17 	REN 	Remote Enable 
6 	DAV 	Data Valid 					18 	Shield 	Ground (DAV) 
7 	NRFD 	Not Ready For Data 			19 	Shield 	Ground (NRFD) 
8 	NDAC 	Not Data Accepted 			20 	Shield 	Ground (NDAC) 
9 	IFC 	Interface Clear 			21 	Shield 	Ground (IFC) 
10 	SRQ 	Service Request 			22 	Shield 	Ground (SRQ) 
11 	ATN 	Attention 					23 	Shield 	Ground (ATN) 
12 	Shield 	Chassis Ground 				24 	Single GND 	Single Ground
*/
#define REN PORTDbits.RD0
#define EOI PORTDbits.RD1
#define DAV PORTDbits.RD2
#define NRFD PORTDbits.RD3
#define NDAC PORTDbits.RD4
#define IFC PORTDbits.RD5
#define SRQ PORTDbits.RD6
#define ATN PORTDbits.RD7
#define INPUT 1
#define OUTPUT 0
#define REN_IO TRISDbits.TRISD0
#define EOI_IO TRISDbits.TRISD1
#define DAV_IO TRISDbits.TRISD2
#define NRFD_IO TRISDbits.TRISD3
#define NDAC_IO TRISDbits.TRISD4
#define IFC_IO TRISDbits.TRISD5
#define SRQ_IO TRISDbits.TRISD6
#define ATN_IO TRISDbits.TRISD7

#define TIMEOUT 40000
#define SMALLDELAY 10

unsigned char IEEE_GetStatus(void){
    
    unsigned char temp = PORTD;
    return temp;

}

void IEEE488_AssertREN(unsigned char state){
    if(state == 1){
		REN_IO=OUTPUT;		// Assert REN
		REN = 0;
	}else{
		REN_IO=INPUT;		// Deassert REN
		REN=1;
	}
       
}

void IEEE488_Init(void){
    // This is the main function to call to setup the gpib
    unsigned int k;
    TRISD=0xFF;
    PORTD=0xFF;
    TRISA=0xFF;
    PORTA=0xFF;
    // both ports inputs to start
    
     for (k=0;k<640;k++);
    IEEE488_AssertREN(1);
    IEEE488_Reset();
    //IEEE488_Reset();

}

void IEEE488_Reset(void){
    // interface clear IFC
    unsigned int k;
    // for (k=0;k<320;k++) is about 1 ms with at 16MHz FOSC
    
    
    IFC_IO=OUTPUT;
    IFC=0;
        
	//
	// Hold IFC low for >100us
	
    for (k=0;k<620;k++);
    
    IFC=1;
    IFC_IO=INPUT;
    
	for (k=0;k<620;k++);
	
    
}

//http://bitsavers.org/pdf/hp/hpib/TutorialDescrOfHPIB.pdf
// page 14 shows hand-shake data transfer process.

char IEEE488_Read_Data(char *buffer, char max, unsigned int *rtn, char terminator){
    unsigned int i,k;
    unsigned long j;
    char error_code=0;
    char done=0;
    i=0;
    NRFD=1;
    NDAC=0;
    NRFD_IO=OUTPUT;
    NDAC_IO=OUTPUT;
    DAV_IO=INPUT;
    EOI_IO=INPUT;
    
    while ((!done)&(error_code==0)){
           for (k=0;k<SMALLDELAY;k++);//small delay
    
        NRFD=1;     
       j=0;
        while ((DAV==1)&(j<TIMEOUT)& (error_code==0)){
            j++;
        }
        if (j==TIMEOUT){
            error_code=0x1A;
                }
       NRFD=0;
       buffer[i] = ~PORTA;
       if (terminator==0){
           done=!(EOI);
       } else {
           done=(buffer[i]==terminator);
       }
       i++;
       if (i==max) error_code=0x40;
       NDAC=1;
       j=0;
       while ((DAV==0)&(j<TIMEOUT)& !(done)& (error_code==0)){
            j++;
        }
        if (j==TIMEOUT){
            error_code=0x2B;
       }
       NDAC=0;
      
    }
    *rtn=i;
    NRFD_IO=INPUT;
    NDAC_IO=INPUT;
    return error_code;;
}

char IEEE488_Write_Data(char *text, char com, char num){
    unsigned short k,i;
    unsigned long j;
    char error_code=0;
    ATN_IO=OUTPUT;
    
// the char com tells if this is a buss command, or data
// see http://www.ni.com/white-paper/3389/en/ for list of bus commands
 // Table 1 
    ATN=(com&0x01);
 // if com=1 then ATN=1 and the information in text[] is treated as specific data to an instrument
    // if com=0 then ATN=0 and the information is system bus commands.
    
    for (k=0;k<SMALLDELAY;k++);//small delay

    NRFD_IO=INPUT;
    NDAC_IO=INPUT;
    if (!(NRFD & NDAC)) {      // if there is a listener, one of these will be low
        for (k=0;k<num;k++){
            // place data on bus
            PORTA = ~text[k];
            TRISA = 0x00;// set data lines output
        for (i=0;i<(2*SMALLDELAY);i++);       
        // wait for NRFD
        j=0;
        while ((NRFD==0)&(j<TIMEOUT)& (error_code==0)){
            j++;
        }
        if (j==TIMEOUT){
            error_code=0x20;
            k=num;
        }
       //set DAV low
        DAV_IO=OUTPUT; // data valid
        DAV=0;
        // wait for NDAC
        j=0;
        while ((NDAC==0)&(j<TIMEOUT)& (error_code==0)){
            j++;
        }
        if (j==TIMEOUT){
            error_code=0x40;
            k=num;
        }
        DAV=1;
           
    }// next byte
         DAV_IO=INPUT; // data valid
    } else {
       error_code=0x80; // there does not appear to be a listener?
    }
     
    // set data lines input
    TRISA=0xFF;
    ATN=1;
    ATN_IO=INPUT;
    return error_code;
}

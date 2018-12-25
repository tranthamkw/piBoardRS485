

#include "mcc_generated_files/mcc.h"
#include "simpleGFX.h"
#include <string.h>
#include "RS485.h"
#include "cfaf128.h"
#include "IEEE488.h"

#include "mcc_generated_files/memory.h"

#define BASEREG 0x0C0C
#define FNREG 0x00F0
#define MYFUNCTION "485-GPIB Bridge"


unsigned char address;
char outText[128];
char tempText[3];
 
void printData(char* data, char num){
    unsigned int i;
   
    outText[0]=0;
     for (i=0;i<num;i++){
        charToHex(data[i],tempText,2);
        strcat(outText,tempText);
        strcat(outText," ");
    }
    i=strlen(outText);
    printLine(outText,i,WHITE,BLACK); 
}



void drawStartupScreen(void){
    char k;
    
    clearLine(BLACK);
    strcpy(outText,"RS485Address:");
    k=strlen(outText);
    writeText(2,outText,0,k,CYAN,BLACK);
    charToHex(address,outText,2);
    k=strlen(outText);
    writeText(17,outText,0,k,CYAN,BLACK);
    updateLine(1);
    
    clearLine(BLACK);
    strcpy(outText," = RS485 to GPIB =");
    k=strlen(outText);
    writeText(0,outText,0,k,YELLOW,BLACK);
    updateLine(3);
    clearLine(BLACK);
    strcpy(outText,"     = BRIDGE =");
    k=strlen(outText);
    writeText(0,outText,0,k,YELLOW,BLACK);
    updateLine(2);
    
}


/*
                         Main application
 */
void main(void)
{
    unsigned int i,k,reg,strlength;
	unsigned short error;
	char RS485data[68];
	char RS232data[68];
	char fnString[16];
    unsigned long timeOut = 0x020000;
    unsigned short numReads=1;
    char debugPrint=0;
	unsigned long timeOutCounter;
	unsigned short temp;

    // Initialize the device
    SYSTEM_Initialize();
    PIN_MANAGER_IOC();
    RS485_Init();
   IEEE488_Init();
    initDisplay();
   
    //DATAEE_WriteByte(0,address);
    address= DATAEE_ReadByte(0);
    drawStartupScreen();
    
    
    while (1)
    {
       if (RS485_Data_Ready()==1){
		strlength = RS485_Read_Data(RS485data,60);
        if (validateRTU(RS485data,strlength)==0){
          /*  
           * ENABLE THIS PORTION FOR VERSION 4 BOARD
           * if (PORTBbits.RB1==0){
                // change the address
                address=RS485data[0];
                DATAEE_WriteByte(0,address);
                
            }*/
            if (RS485data[0]==address){
                switch (RS485data[1]){  // command code byte
					case 0x06: // write a register  - USED TO "RECONFIGURE" THE DEVICE.
						reg = (RS485data[2]<<8) | (RS485data[3]);
							switch (reg){
								case BASEREG: //manual write to ports
                                    /* this is not normally used but provided to manually force the outputs */
                                    if (strlength==8){
                                        TRISD=0;
                                        TRISA=0;
										PORTD=RS485data[4];
                                        PORTA=RS485data[5];
										// formulate a response echo back command
										strlength=6;
                                        RS485_Write_Data(RS485data,strlength);
                                       	error = 0;
                                    }else {
										error = 0x0A;
                                    } // wrong format?		
                                    break;
                                case BASEREG+3:  // initialize the GPIB
									temp = RS485data[5];  // 
									if (temp<2){
										IEEE488_Init();
                                        if (debugPrint){strcpy(outText,"INIT GPIB");
                                        k=strlen(outText);
                                        printLine(outText,k,BLACK,GREEN);}
										// formulate a response echo back command
										strlength=6;
                                        RS485_Write_Data(RS485data,strlength);
										error = 0;
										} else {
										error = 0x0A; } // out of range										}
								break;
                                case BASEREG+4:  // change debug print.  Turn this on to have device print to local screen
                                        temp = RS485data[5];  // assign new data to variable ("register")
                                        if (temp<2){
                                           debugPrint=temp;
                                        if (debugPrint){strcpy(outText,"Change Debug Print");
                                        k=strlen(outText);
                                        printLine(outText,k,BLACK,GREEN);}

                                            // formulate a response echo back command
                                            strlength=6;
                                            RS485_Write_Data(RS485data,strlength);
                                            error = 0;
										}else {
                                            error = 0x0A; // out of range
										}
								break;
								case FNREG:  // writing to this changes the RS485 address
										if (RS485data[4]==address){
											address=RS485data[5];
                                            DATAEE_WriteByte(0,address);
											strlength=6;
                                           	RS485_Write_Data(RS485data,strlength);
                                            if (debugPrint){
                                            printData(RS485data,strlength);
                                            strcpy(outText,"NEW RS485 Address ");
                                            charToHex(address,tempText,2);
                                            strcat(outText,tempText);            
                                            k=strlen(outText);
                                            printLine(outText,k,YELLOW,BLACK);}
                                            error=0;
										} else {
										error = 0x06;
										}
									break;
                                case BASEREG+16:  // write GPIB command
                                   // printData(RS485data,strlength);
									strlength=strlength-2; // ignore last two elements as they are CRC's
									for (i=4; i<strlength; i++){ //ignore first four bytes: ADDRESS 0x06 REGh REGl 
										RS232data[i-4]=RS485data[i];//copy to RS232 buffer
									}
									strlength=strlength-4;  
								//rs232data is 5 bytes shorter since ignore channel and command bytes
                                    if (debugPrint){
                                    strcpy(outText,"GPIB CMD TX:");
                                    k=strlen(outText);
                                    printLine(outText,k,BLACK,GREEN);
                                    printLine(RS232data,strlength,BLACK,GREEN);
                                    printData(RS232data,strlength);}
                                    
                                    error = IEEE488_Write_Data(RS232data,0,strlength);
                                    
                                    if (error==0){
								        strlength=strlength+4;
                                        RS485_Write_Data(RS485data,strlength);//just pump back the same message with error bit not set
									    }
                                    
                                    break;// END WRITE TO write GPIB COMMAND
                                    
								case BASEREG+32: // write GPIB DATA
                                    strlength=strlength-2; // ignore last two elements as they are CRC's
									for (i=5; i<strlength; i++){ //ignore first five bytes: ADDRESS 0x06 REGh REGl GPIBaddress
										RS232data[i-5]=RS485data[i];//copy to RS232 buffer
									}
									strlength=strlength-5;  
								//rs232data is 5 bytes shorter since ignore channel and command bytes
                                    tempText[0]=0x20 | RS485data[4]; //RS485data[4] holds the GPIB address. 
                                    IEEE488_Write_Data(tempText,0,1); // tell device to listen
                                 if (debugPrint){
                                    strcpy(outText,"GPIB DATA TX:");
                                    k=strlen(outText);
                                    printLine(outText,k,GREEN,BLACK);
                                    printLine(RS232data,strlength,GREEN,BLACK);
                                    printData(RS232data,strlength);}
                                  
                                    error = IEEE488_Write_Data(RS232data,1,strlength);
                                    
                                    if (error==0){
								        strlength=strlength+5;
                                        RS485_Write_Data(RS485data,strlength);//just pump back the same message with error bit not set
                                        }
                                    tempText[0]=0x3F;//UNL
                                    IEEE488_Write_Data(tempText,0,1); // tell all devices to unlisten
								break;// END WRITE TO write to GPIB
							default:
									// invalid register
							error = 0x02;
							}
						break; // end write to a register
					case 0x03: // read from a register
					reg = (RS485data[2]<<8) | (RS485data[3]);
							switch (reg){
								
                                case BASEREG+3: // read GPIB status
									RS485data[2]=2;
									RS485data[3]=0; RS485data[4]=IEEE_GetStatus();  // assign new data to varible ("register")
										strlength=5;
                                        RS485_Write_Data(RS485data,strlength);
										error = 0;
								break;
                                case BASEREG+4: // read status of debug print to local screen
									RS485data[2]=2; 
									RS485data[3] = 0;
									RS485data[4] = (debugPrint&0x00FF);
										strlength=5;
                                   		RS485_Write_Data(RS485data,strlength);
										error = 0;
								break;
                                case (BASEREG+32): // have the GPIP talk 
									/* data[0]=RS485address
                                     data[1]=0x03
                                     data[2]= MSB reg
                                     data[3]=LSB reg
                                     data[4]=GPIB address of device that should talk
                                     data[5]=terminator to look for*/
                                    
                                    tempText[0]=0x40 | RS485data[4];
                                    IEEE488_Write_Data(tempText,0,1); // tell device to talk
                                    
                                    error=IEEE488_Read_Data(RS232data, 64, &strlength,  RS485data[5]);  // then read data
                                   if (debugPrint){ strcpy(outText,"NUM RETURN BYTES:");
                                    charToHex(strlength,tempText,2);
                                    strcat(outText,tempText);
                                    i=strlen(outText);
                                    printLine(outText,i,GREEN,BLACK); 
                                    strcpy(outText,"GPIB RX:");
                                    k=strlen(outText);
                                    printLine(outText,k,GREEN,BLACK);
                                    printLine(RS232data,strlength,GREEN,BLACK);
                                    printData(RS232data,strlength);}
                                    
                                    if (error==0){
                                        //copy RS232data to 485data
                                        RS485data[2] = strlength;
                                        for (i=3; i<(strlength+3); i++){
                                            RS485data[i]=RS232data[i-3];
                                        }
                                        strlength +=3;
                                        RS485_Write_Data(RS485data,strlength);
                                        }
                                    tempText[0]=0x5F;//UNT
                                    IEEE488_Write_Data(tempText,0,1); // tell all devices to untalk
								break;
								case FNREG:
								strcpy(fnString,MYFUNCTION);
								strlength = strlen(fnString);
								//data[0]=<RS485Address>; //data[1]=03;
                                RS485data[2] = (unsigned char)strlength;// number of bytes to follow
								for (i=0;i<strlength;i++){
										RS485data[3+i]=fnString[i];
										}
								strlength+=3;
                                RS485_Write_Data(RS485data,strlength);
                                error =0;
								break;
                                default:
									// invalid register
								error = 0x02;
								}
							break;//read a register
							default:
								// unknown command
								error = 0x08;
							break;
						}//end switch
                
                if ((error==0)&(debugPrint)){strcpy(outText,"RS485 TX:");
                k=strlen(outText);
                printLine(outText,k,CYAN,BLACK);
                printLine(RS485data,strlength,CYAN,BLACK);}

				if (error) {
					RS485data[1]=RS485data[1] | 0x80;
					RS485data[2]= (char)((error & 0xFF00) >> 8);
					RS485data[3]=(error & 0x00FF);
					strlength=4;
					RS485_Write_Data(RS485data,strlength);
                   if (debugPrint){ strcpy(outText,"ERROR");
                    k=strlen(outText);
                    printLine(outText,k,BLACK,RED);
                    switch (error){
                        case 0x02: strcpy(outText,"INVALID REGISTER");
                        break;
                        case 0x06: strcpy(outText, "VALUE OUT OF RANGE");
                        break;
                        default: strcpy(outText,"UNKNOWN COMMAND");
                    }
                    k=strlen(outText);
                    printLine(outText,k,RED,BLACK);
                    strcpy(outText,"Error Code: ");
                    charToHex(error,tempText,2);
                    strcat(outText,tempText);
                    i=strlen(outText);
                    printLine(outText,i,RED,BLACK); }
                    
                    }

            } // if our address.
            //invalid RTU  just ignore the message
			}
       }//end RS485 data ready
     
    }
}
/**
 End of File
*/
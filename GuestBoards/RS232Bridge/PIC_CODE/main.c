

#include "mcc_generated_files/mcc.h"
#include "simpleGFX.h"
#include <string.h>
#include "RS485.h"
#include "cfaf128.h"
#include "RS232.h"
#include "mcc_generated_files/memory.h"

#define BASEREG 0x0C0C
#define FNREG 0x00F0
#define MYFUNCTION "485-232Bridge"


unsigned char address=0xC4;
char outText[128];
char tempText[3];
 
void printData(char* data, char num, unsigned short fcolor, unsigned short bcolor){
    unsigned int i,k;
    
    k=strlen(outText);
    printLine(outText,k, fcolor,bcolor);
    printLine(data,num, fcolor,bcolor);
    
    strcpy(outText,"");
     for (i=0;i<num;i++){
        charToHex(data[i],tempText,2);
        strcat(outText,tempText);
        strcat(outText," ");
    }
    i=strlen(outText);
    printLine(outText,i,fcolor, bcolor); 
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
    strcpy(outText,"   RS485 to RS232");
    k=strlen(outText);
    writeText(0,outText,0,k,YELLOW,BLACK);
    updateLine(3);
    clearLine(BLACK);
    strcpy(outText,"       BRIDGE");
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
    RS232_Init();
    initDisplay();
   
   // DATAEE_WriteByte(0,address);
    address= DATAEE_ReadByte(0);
    drawStartupScreen();
    
    
    while (1)
    {
       if (RS485_Data_Ready()==1){
		strlength = RS485_Read_Data(RS485data,60);
        if (validateRTU(RS485data,strlength)==0){
            if (PORTBbits.RB1==0){
                // change the address
                address=RS485data[0];
                DATAEE_WriteByte(0,address);
                
            }
            error=0;
            if (RS485data[0]==address){
                 if (debugPrint){
                                 strcpy(outText, "RS485 Rx");
                                 printData(RS485data, strlength, BLACK,CYAN); 
                                }
                switch (RS485data[1]){  // command code byte
					case 0x06: // write a register  - USED TO "RECONFIGURE" THE DEVICE.
						reg = (RS485data[2]<<8) | (RS485data[3]);
							switch (reg){
								case BASEREG+1:
									temp = RS485data[5];  // assign new data to variable ("register")
									if ((temp<8)&(temp>0)){
										numReads=temp;
										// formulate a response echo back command
										strlength=6;
                                        RS485_Write_Data(RS485data,strlength);
                                       
										error = 0;
										}else {
										error = 0x06; // out of range
										}
								break;
                                case BASEREG+4:
									temp = RS485data[5];  // assign new data to variable ("register")
									if (temp<2){
										debugPrint=temp;
										// formulate a response echo back command
										strlength=6;
                                        RS485_Write_Data(RS485data,strlength);
                                       
										error = 0;
										}else {
										error = 0x06; // out of range
										}
								break;
								case BASEREG+2:
									timeOutCounter = (((unsigned long) RS485data[4]<<16)|RS485data[5]<<8);  
									if (timeOutCounter>0){
										timeOut=timeOutCounter;
										strlength=6;
                                        RS485_Write_Data(RS485data,strlength);
                                       
                                        error = 0;
										}else {
										error = 0x06; // out of range
										}
								break;
                                	case FNREG:
										if (RS485data[4]==address){
											address=RS485data[5];
                                            DATAEE_WriteByte(0,address);
											strlength=6;
                                           	RS485_Write_Data(RS485data,strlength);
                                           
                                             strcpy(outText,"RS485 Address ");
                                             charToHex(address,tempText,2);
                                             strcat(outText,tempText);            
                                            k=strlen(outText);
                                            printLine(outText,k,YELLOW,BLACK);
                                            error=0;
										} else {
										error = 0x02;
										}
									break;
								case (BASEREG+32):
                                                                     
									strlength = strlength-2; // ignore last two elements as they are CRC's
									for (i=4; i<strlength; i++){ //ignore first four bytes: ADDRESS 0x06 REGh REGl
										RS232data[i-4]=RS485data[i];//copy to RS232 buffer
									}
									strlength=strlength-4;  
								//rs232data is 4 bytes shorter since ignore channel and command bytes
                                    if (debugPrint) {
                                        strcpy(outText,"RS232 TX:");
                                        printData(RS232data,strlength, BLACK, GREEN); }
                                    RS232_Write_Data(RS232data,strlength); 
								for (k=0;k<numReads;k++){
                                    timeOutCounter=0;
                                    strlength=0;
                                    do{
								        if (RS232_Data_Ready()==1){
                                        strlength = RS232_Read_Data(RS232data,54);
                                        timeOutCounter=timeOut; // message received so quit
                                        }
                                    timeOutCounter++;
                                    } while (timeOutCounter<timeOut);
                                    if (strlength==54) RS232data[53]=178;
								}
								if (strlength==0){
											// no response from 232 device
									strcpy(RS232data,"NO RESPONSE FROM RS232 DEVICE");
									strlength=strlen(RS232data);
								}		
                                 if (debugPrint) {
                                        strcpy(outText,"RS232 RX:");
                                        printData(RS232data,strlength, GREEN,BLACK); }
							//copy to rs485 buffer. Address,command code is still saved data[0],data[1]
								RS485data[2] = strlength;
									for (i=3; i<(strlength+3); i++){
										RS485data[i]=RS232data[i-3];
									}
									strlength +=3;
									RS485_Write_Data(RS485data,strlength);
									error = 0;
                                   
								break;// END WRITE TO write to RS232
							default:
									// invalid register
							error = 0x02;
							}
						break; // end write to a register
					case 0x03: // read from a register
					reg = (RS485data[2]<<8) | (RS485data[3]);
							switch (reg){
								case BASEREG+1:
									RS485data[2]=2;
									RS485data[3]=0; RS485data[4]=numReads;  // assign new data to varible ("register")
										strlength=5;
                                         
										RS485_Write_Data(RS485data,strlength);
										error = 0;
								break;
								case BASEREG+2:
									RS485data[2]=2;
									RS485data[3] = ((timeOut&0xFF0000)>>16);
									RS485data[4] = ((timeOut&0x00FF00)>>8);
										strlength=5;
                                        
										RS485_Write_Data(RS485data,strlength);
										error = 0;
								break;
                                case BASEREG+4:
									RS485data[2]=2;
									RS485data[3] = 0;
									RS485data[4] = ((debugPrint&0x00FF));
										strlength=5;
                                        
										RS485_Write_Data(RS485data,strlength);
										error = 0;
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
                if ((debugPrint)&!(error)){
                                 strcpy(outText, "RS485 Tx");
                                 printData(RS485data, strlength, CYAN, BLACK); 
                                }
				if (error) {
					RS485data[1]=RS485data[1] | 0x80;
					RS485data[2]= (char)((error & 0xFF00) >> 8);
					RS485data[3]=(error & 0x00FF);
					strlength=4;
					RS485_Write_Data(RS485data,strlength);
                    if (debugPrint) { strcpy(outText,"RS485 ERROR");
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
                    printLine(outText,k,RED,BLACK);}
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
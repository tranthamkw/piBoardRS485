/* 
 * File:   RS485.h
 * Author: tranthamkw
 *
 * Created on July 5, 2018, 5:04 PM
 */

#ifndef RS485_H
#define	RS485_H

#ifdef	__cplusplus
extern "C" {
#endif

//#include "mcc_generated_files/eusart1.h"
   #include <xc.h>
   
    
unsigned short modRTU_CRC(char* buff, int len);
int validateRTU(char* buff, int len);
void RS485_Init(void);
char RS485_Data_Ready(void);
void resetRS485(void);
int RS485_Read_Data(char *buffer,char max);
void RS485_Write_Data(char *text, char num);

#ifdef	__cplusplus
}
#endif

#endif	/* RS485_H */


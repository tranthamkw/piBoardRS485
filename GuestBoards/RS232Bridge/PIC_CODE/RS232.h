/* 
 * File:   RS232.h
 * Author: tranthamkw
 *
 * Created on July 11, 2018, 8:45 PM
 */

#ifndef RS232_H
#define	RS232_H

#ifdef	__cplusplus
extern "C" {
#endif


//#include "mcc_generated_files/eusart2.h"
   #include <xc.h> 

void RS232_Init(void);
char RS232_Data_Ready(void);
int RS232_Read_Data(char *buffer,char max);
void RS232_Write_Data(char *text, char num);

#ifdef	__cplusplus
}
#endif

#endif	/* RS232_H */


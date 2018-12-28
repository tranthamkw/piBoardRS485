/* 
 * File:   IEEE488.h
 * Author: tranthamkw
 *
 * Created on August 19, 2018, 9:45 AM
 */

#ifndef IEEE488_H
#define	IEEE488_H

#ifdef	__cplusplus
extern "C" {
#endif

unsigned char IEEE_GetStatus(void);
void IEEE488_Init(void);
void IEEE488_Reset(void);
char IEEE488_Read_Data(char *buffer, char max, unsigned int *rtn, char terminator);
char IEEE488_Write_Data(char *text, char com, char num); 


#ifdef	__cplusplus
}
#endif

#endif	/* IEEE488_H */


#include <pic.h>

#define _XTAL_FREQ 8000000


unsigned short modRTU_CRC(char* buff, int len);
int validateRTU(char* buff, int len);
char RS485_Init();
char RS485_Data_Ready();
int RS485_Read_Data(char *buffer,char max);
void RS485_Write_Data(char *text, int num);
void RS232_Write_Data(char *text, int num);



/*


 */
#include "SRSLockIn.h"

//assumes uses the RS232<->RS485 bridge
int getSRS830Data(float* r,float* phi, float* f, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;
	float tempA;

	strcpy(cmdData, "SNAP?3,4,9");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;

	i=writeRS485to232Bridge(cmdData,echoData,RS485Address);
	// we are expecting a sequence of three numbers seperated by commas
	// printf("Return: %s\n",echoData);

char *ptr = strtok(echoData,",");


	//printf("%s \t",ptr);
tempA = atof(ptr);
*r = tempA;
	//printf("%f \n",tempA);

ptr = strtok(NULL,",");
tempA = atof(ptr);
	//printf("%s \t",ptr);
*phi = tempA;
	//printf("%f \n",tempA);


ptr = strtok(NULL,",");
	//printf("%s \t",ptr);
tempA = atof(ptr);
*f = tempA;
	//printf("%f \n",tempA);
return i;
}

int initSRS830(unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;

	// OUTX sets the default interface that the instrument talks with.
	// 0 = RS232
	// 1 = GPIB

	strcpy(cmdData, "OUTX0;PHAS0");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;

	writeRS485to232Bridge(cmdData,echoData,RS485Address);
}

int initSRS530(unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;

	strcpy(cmdData, "W0;S2;P0");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	i = writeRS485to232Bridge(cmdData,echoData,RS485Address);
return i;
}

int getSRS530R(float* r, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;
	float tempA;

	strcpy(cmdData, "Q1");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	i = writeRS485to232Bridge(cmdData,echoData,RS485Address);
	tempA = atof(echoData);
	*r = tempA;

return i;

}

int getSRS530Phi(float* phi, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;
	float tempA;

	strcpy(cmdData, "Q2");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	i = writeRS485to232Bridge(cmdData,echoData,RS485Address);
	tempA = atof(echoData);
	*phi = tempA;

return i;

}

int getSRS530Freq(float* f, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;
	float tempA;

	strcpy(cmdData, "F");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	i = writeRS485to232Bridge(cmdData,echoData,RS485Address);
	tempA = atof(echoData);
	*f = tempA;

return i;

}


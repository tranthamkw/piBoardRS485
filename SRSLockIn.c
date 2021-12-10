
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
	unsigned char cmdData[128];
	unsigned char echoData[128];
	int i;

	/*
	 OUTX sets the default interface that the instrument talks with.
		 0 = RS232
		 1 = GPIB
	PHAS0 sets the reference phase shift to zero
	OFLT9 sets time constant to 300mS
	OFSL1 
	RMOD2 sets the dynamic reserve to low noise
	DDEF1,1,0 and DDEF2,1,0 sets the displays to R and theta
	RSLP1 sets the reference triger to TTL rising edge
	HARM1 set the detection harmonic to the fundamental of the reference frequency

	*/
	strcpy(cmdData, "OUTX0;OFLT9;PHAS0;RMOD2");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	writeRS485to232Bridge(cmdData,echoData,RS485Address);

	strcpy(cmdData, "DDEF1,1,0;DDEF2,1,0;RSLP1;HARM1");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	writeRS485to232Bridge(cmdData,echoData,RS485Address);


}

int initSRS530(unsigned short RS485Address){
	unsigned char cmdData[64];
	unsigned char echoData[128];
	int i;
		/*

	W0 sets the RS232 wait interval to zero. Default is 6. for slow computers.
	S2 sets the displays to R and theta
	P0 sets the reference phase shift to 0
	D0 sets the dynamic reserve to low noise
	T1,6 sets the time constant to 300ms
	R0 sets the trigger mode to positive
	*/
	strcpy(cmdData, "W0;S2;P0;D0;T1,6;R0");
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

int getSRS335Freq(float*f,unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;
	float tempA;

	strcpy(cmdData, "FREQ?");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	i = writeRS485to232Bridge(cmdData,echoData,RS485Address);
	tempA = atof(echoData);
	*f = tempA;
}
int setSRS335Freq(float f, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;

	sprintf(cmdData,"FREQ%.3f",f);

	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;

	i = writeRS485to232Bridge(cmdData,echoData,RS485Address);

}


int getSRS335Ampl(float*f,unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char echoData[128];
	int i;
	float tempA;

	strcpy(cmdData, "AMPL?");
	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
	i = writeRS485to232Bridge(cmdData,echoData,RS485Address);
	// we are expecting a sequence of numbers follwed by VP
	// printf("Return: %s\n",echoData);

	char *ptr = strtok(echoData,"V");
	//printf("%s \t",ptr);
	tempA = atof(ptr);
	*f = tempA;
	//printf("%f \n",tempA);
}


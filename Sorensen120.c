
/*


 */
#include "Sorensen120.h"



int getSorensen120Volts(float* volts,char gpibaddress, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char chardata[64];
	float tempA=0.0;
	unsigned int i;
	unsigned int len;
	strcpy(cmdData,"VOUT?");
	i=strlen(cmdData);
	cmdData[i]=0x0A;
	cmdData[i+1]=0;
	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);
	if (!status) status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);

	if (status==0) {// no errors
		//  format of return data 
		// VOUT 12.9<CR><LF>
		// this strips the first four characters
		len = strlen(chardata);
		for (i=0;i<(len-4);i++) chardata[i]=chardata[i+4];
		tempA = atof(chardata);
//		printf("returnstring %s\t",chardata);
//		printf("atof conversion: %f\n",tempA);
		}

	*volts = tempA;

	return status;
}
int getSorensen120Amps(float* amps,char gpibaddress, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned char chardata[64];
	float tempA=0.0;
	unsigned int i;
	unsigned int len;
	strcpy(cmdData,"IOUT?");
	i=strlen(cmdData);
	cmdData[i]=0x0A;
	cmdData[i+1]=0;
	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);
	if (!status) status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);

	if (status==0) {// no errors
		//  format of return data 
		// VOUT 12.9<CR><LF>
		// this strips the first four characters
		len = strlen(chardata);
		for (i=0;i<(len-4);i++) chardata[i]=chardata[i+4];
		tempA = atof(chardata);
//		printf("returnstring %s\t",chardata);
//		printf("atof conversion: %f\n",tempA);
		}

	*amps = tempA;

	return status;
}

int setSorensen120Volts(float volts, char gpibaddress, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned int i;

	sprintf(cmdData,"VSET %.1f",volts);
//	printf("test print command: %s\n",cmdData);
	i=strlen(cmdData);
	cmdData[i]=0x0A;
	cmdData[i+1]=0;
	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);
return status;
}


int initSorensen120(char gpibaddress, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned int i;
	strcpy(cmdData,"ISET 0.4");// default power on is ISET 0.0.  This causes the output to be zero regardlesss of any VSET commend
// all other power defaults should be good
// see page 22 of the Sorensen manual
	i=strlen(cmdData);
	cmdData[i]=0x0A;
	cmdData[i+1]=0;
	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);
return status;

}

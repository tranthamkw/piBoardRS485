/*


 */

#include "Fluke8840.h"

int getReadingF8840(float* volts, char gpibaddress, unsigned short RS485Address){

	unsigned char chardata[64];
	float tempA=0.0;
	unsigned int i;

	int status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);

	if (status==0) {// no errors
		// format of F8840 is numeric with suffix turned off.  See page 3-25 of the manual
		tempA=atof(chardata);
//		printf("F8840returnstring %s\t",chardata);
//		printf("atof conversion: %f\n",tempA);

		}else{
		status = chardata[0];
		}
	
	*volts=tempA;
	return status;
}

int initializeF8840(char gpibaddress, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned int i;
	strcpy(cmdData,"F1R0Y0");
	i=strlen(cmdData);
	cmdData[i]=0x0D;// terminator 
	cmdData[i+1]=0;
// Page 3-7 of manual, FLUKE 8840a
// F1 function VDC
// R4 200V full scale
//R0 autoscale
// Y0 dont send suffix
	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);

return status;
}


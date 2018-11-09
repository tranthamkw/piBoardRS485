/*


 */
#include "K485ammeter.h"



int getReadingK485(float* amps,char gpibaddress, unsigned short RS485Address){
	unsigned char chardata[64];
	float tempA=0.0;
	unsigned int i;
	unsigned int len;

	int status = listenGPIBData(chardata, 0x0A, gpibaddress, RS485Address);

	if (status==0) {// no errors
		// format of K485 return string.  See page 3-14 of the manual
		//  NDCA±1.2345E-9<CR><LF>
		// initialize function turns off prefix, so just atof the returned value
//		len = strlen(chardata);
//		for (i=0;i<len;i++) chardata[i]=chardata[i+1];
		tempA = atof(chardata);
//		printf("k485returnstring %s\t",chardata);
//		printf("atof conversion: %f\n",tempA);
		}else{

		status = chardata[0];
		}

	*amps = tempA;

	return status;
}


int initializeK485(char gpibaddress, unsigned short RS485Address){
	unsigned char cmdData[16];
	unsigned int i;
	strcpy(cmdData,"G1R0X");

	i=strlen(cmdData);
	cmdData[i]=0x0D;
	cmdData[i+1]=0;
// G1 reading without prefix
// set range to auto scale. the rest of the power-on defaults are fine.
// see page 3-10
// R0 is autorange.  I dont know if the machine will 'talk' or return valid data when it is in the process of autoranging.

	int status = sendGPIBData(cmdData,gpibaddress, RS485Address);

return status;

}

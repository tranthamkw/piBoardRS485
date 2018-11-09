/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "piBoard.h"
#include "RS485Devices.h"
//#include "omegaCN7500.h"
//#include "K485ammeter.h"
//#include "Fluke8840.h"
//#include "BK1696.h"
//#include "Sorensen120.h"

// THESE ARE THE RS485 ADDRESSES OF EACH RS485 INSTRUMENT/BRIDGE/SLAVE DEVICE
//#define OMEGA1 6 // the omega CN7800 are native RS485 Modbus speakers
//#define OMEGA2 8
//#define ANALOGRECORD 0xA1 // an analog recorder slave
//#define SERVO1 0xD1 // dual servo device

//#define GPIBBRIDGE1 0XC1 // the gpib bridge can have many gpib devices attached to it, so will also need the GPIB address of each
// this is the GPIB addresses of each respective instrument attached to this bridge
//#define K485AMMETER 0X0A
//#define FLUKE8840 0x06
//#define SORENSEN120 0x0C

//#define GPIBBRIDGE2 0xC8 //Gpib bridge #2 with the following instruments attached
//#define SRSLOCKIN 0x08
//#define K485AMMETER 0x0A.  Although this is the same GPIB address as the above ammeter, the second ammeter
//is on a different GPIB bus.

//#define RS232BRIDGE1 0xC4 // only one instrument can be attached to a 232 bridge. this one happens to be a BK1696 power supply.
//I suppose #define BK1696 0xC4 would be just as informative. or #define TARGETPOTENTIAL 0xC4 ... if this BK controlled "target"



int main (int argc, char* argv[]){
	int i,k;
	float myTemp,myVolts;
	unsigned int periods, valueADC;

	initializeBoard();
/*
	setRS485ServoPosition(SERVO1, 0, 0);
	printf("Sending IFC to GPIB instruments on bridge %02x\n",GPIBBRIDGE1);
	i=resetGPIBBridge(GPIBBRIDGE1);
	printf("Status %d\n",i);
	delay(200);
	i=initializeK485(K485AMMETER,GPIBBRIDGE1);
	printf("Init K485\nStatus %d\n",i);
	i=initializeF8840(FLUKE8840,GPIBBRIDGE1);
	printf("Init Fluke 8840\nStatus %d\n",i);
	i=initSorensen120(SORENSEN120,GPIBBRIDGE1);
	printf("Init Sorensen\nStatus %d\n",i);
	i=initializeBK1696(RS232BRIDGE1);
	printf("Init BK1696 instrument #1\nStatus %d\n",i);

	i=getPVCN7500(OMEGA1,&myTemp);
	if (!i) printf("Omega 1 temperature = %.1f°\n",myTemp); else printf("Status %d\n",i);
	i=getPVCN7500(OMEGA2,&myTemp);
	if (!i) printf("Omega 2 temperature = %.1f°\n",myTemp); else printf("Status %d\n",i);

	i = getRS485AnalogRecorderPeriod(ANALOGRECORD, &periods);
	printf("Analog recorder sampling every %dmS\n",periods*16);
	for (k=0; k<4; k++){
	i=readRS485AnalogRecorder(ANALOGRECORD, k, 5.0, &myVolts, &myTemp);
	delay(100);
	printf("Analog recorder chan %d = %.2f ± %.2f\n",k,myVolts, myTemp);
	}
	
	i=getReadingK485(&myTemp, K485AMMETER,GPIBBRIDGE1);
	if (!i) printf("K485 reading = %E\n",myTemp);else printf("Status %d\n",i);
	delay(100);
	i=getReadingF8840(&myTemp, FLUKE8840,GPIBBRIDGE1);
	if (!i)	printf("Fluke 8840 reading = %E\n",myTemp);else printf("Status %d\n",i);

	for (k=0;k<8;k++){
	i=setVoltsBK1696(RS232BRIDGE1, (float) k * 1.2+1.1);
	delay(100);
	printf("Set Servo %d\t",k);
	setRS485ServoPosition(SERVO1, 0, k);
	getRS485ServoPosition(SERVO1,0,&i);
	printf("Return pos %d\n",i);
	delay (100);
	i=getReadingF8840(&myTemp, FLUKE8840,GPIBBRIDGE1);
	if (!i) printf("Fluke = %E\t",myTemp); else printf("Status %d\n",i);
	delay (100);
	i=getReadingF8840(&myTemp, FLUKE8840,GPIBBRIDGE1);
	if (!i) printf("Fluke = %E\n",myTemp);else printf("Status %d\n",i);
	}

	for (k=0; k<10; k++){
	myTemp = (float)k * 12.2;
	i = setSorensen120Volts(myTemp,SORENSEN120,GPIBBRIDGE1);
	printf("Setting Sorensen %.1f\t",myTemp);
	delay(200);
	i = getSorensen120Volts(&myVolts,SORENSEN120,GPIBBRIDGE1);
	i = getSorensen120Amps(&myTemp,SORENSEN120,GPIBBRIDGE1);
	printf("Measured: %.2fV\t %.3fA\t %.1fW\n",myVolts,myTemp,myVolts*myTemp);
	delay(200);
	}
	i = setSorensen120Volts(0.0,SORENSEN120,GPIBBRIDGE1);
	setRS485ServoPosition(SERVO1, 0, 0);
*/

for (i=0;i<8;i++){

	getADC(i,&valueADC);
	printf("ADC: %d\t",valueADC);
	delay(100);

}


	return 0 ;
}

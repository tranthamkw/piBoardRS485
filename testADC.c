/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "piBoard.h"
#include "RS485Devices.h"

int main (int argc, char* argv[]){
	int i,k;
	float myTemp,myVolts;
	unsigned int periods, valueADC;

	initializeBoard();
/*
*/

	for (i=0;i<8;i++){

	getADC(i,&valueADC);
	printf("ADC%d: %d\t",i,valueADC);
	delay(100);

	}
	printf("\nOK\n");

	return 0 ;
}

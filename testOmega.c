/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "piBoard.h"
#include "RS485Devices.h"
#include "omegaCN7500.h"
#include "omegaCN740.h"

// THESE ARE THE RS485 ADDRESSES OF EACH RS485 INSTRUMENT/BRIDGE/SLAVE DEVICE
#define OMEGA1 12 // omegaCN740
#define OMEGA2 8 //omegaCN7800




int main (int argc, char* argv[]){
	int i,k;
	float myTemp,myVolts;
	unsigned int periods;
	initializeBoard();






	i=getPVCN7500(OMEGA2,&myTemp);
	if (!i) printf("Omega 2 temperature = %.1f°\n",myTemp); else printf("Status %d\n",i);
delay(100);
	i=getPVCN740(OMEGA1,&myTemp);
	if (!i) printf("Omega 1 temperature = %.1f°\n",myTemp); else printf("Status %d\n",i);

	delay(100);


	return 0 ;
}

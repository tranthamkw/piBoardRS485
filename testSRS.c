/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "piBoard.h"
#include "RS485Devices.h"
#include "SRSLockIn.h"

// THESE ARE THE RS485 ADDRESSES OF EACH RS485 INSTRUMENT/BRIDGE/SLAVE DEVICE
#define SRS830 0xCA
#define SRS530 0xC6
#define SRS345 0xCB


int main (int argc, char* argv[]){



	int i,k,runtime;
	float myTemp,myVolts,myR,myPhi,myf;
	unsigned int periods;
	k=0;

	if (argc==2){
	runtime = atoi(argv[1]);
	}else{
		runtime = 10;
		printf("Using default runtime of 10 seconds\n");
	}


	initializeBoard();
	printf("initializing SRS530   . . .\n");
	initSRS530(SRS530);
	printf("initializing SRS830 . . .\n");
	initSRS830(SRS830);

	printf("SRS830\t\t\t\t\t\tSRS530\n");
	printf("R\t\tPhi\t\tf\t\tR\t\tPhi\t\tf\n");

	do {
		getSRS830Data(&myR,&myPhi,&myf,SRS830);
		printf("%f\t%f\t%f\t",myR,myPhi,myf);
		getSRS530R(&myR,SRS530);
		getSRS530Phi(&myPhi,SRS530);
		getSRS530Freq(&myf,SRS530);
		printf("%f\t%f\t%f\n",myR,myPhi,myf);
		k++;
	} while (k<runtime);

	return 0 ;
}

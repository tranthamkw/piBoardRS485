/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "piBoard.h"
#include "RS485Devices.h"

int main (int argc, char* argv[]){
	int i,k;
	unsigned int steps,dir,state;

	initializeBoard();
/*
*/

	if (argc==3){
		dir = atoi(argv[1]);
		state = atoi(argv[2]);
	} else {
	printf("usage  ./testStepMotor <dir> <state>/n");
	return 1;
	}

	setRS485StepperMotorStepsRev(0xA0,3000);
    delay(100);
	findHomeRS485StepperMotor(0xA0,state,dir);

	delay(100);

printf("\nOK\n");
return 0 ;
}

/*
 * This is a template file to show all of the necessary components to add

test stepmotor

 * to new data collection files that are created. 
*/

#include "piBoard.h"
#include "RS485Devices.h"

int main (int argc, char* argv[]){
	int i,k;
	unsigned int steps,dir,speed;

	initializeBoard();
/*
*/

	if (argc==4){
		steps = atoi(argv[1]);
		dir = atoi(argv[2]);
		speed = atoi(argv[3]);
	} else {
	printf("usage  ./testStepMotor <steps> <dir> <speed 50-255>/n");
	return 1;
	}
	setRS485StepperMotorSpeed(0xA0,speed);

	printf("%x\t%x\n",steps,dir);

	moveRS485StepperMotor(0xA0,steps,dir);

	delay(100);

printf("\nOK\n");
return 0 ;
}

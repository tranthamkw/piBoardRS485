/*
 * This is a template file to show all of the necessary components to add

test digital in

 * to new data collection files that are created. 
*/

#include "piBoard.h"
#include "RS485Devices.h"

int main (int argc, char* argv[]){
	unsigned short speed;

	initializeBoard();
/*

	if (argc==4){
		steps = atoi(argv[1]);
		dir = atoi(argv[2]);
		speed = atoi(argv[3]);
	} else {
	printf("usage  ./testStepMotor <steps> <dir> <speed 50-255>/n");
	return 1;
	}
*/
	getRS485DigitalOut(0xA6,&speed);

	printf("%i\n",speed);

	delay(100);

printf("\nOK\n");
return 0 ;
}

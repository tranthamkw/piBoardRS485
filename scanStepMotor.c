/*
 * This is a template file to show all of the necessary components to add

move steppermotor back 50 steps

for i in range (0,100,2) :
	stepmotor 2
	record encoder

 * to new data collection files that are created. 
*/
#include <time.h>
#include "piBoard.h"
#include "RS485Devices.h"
#include "myFileIO.h"



int main (int argc, char* argv[]){
	int i,k;
	unsigned int steps,dir,speed;
	unsigned short position;

  	time_t rawtime, now;
    double seconds;
    struct tm * timeinfo;
    char appender[BUFSIZE],fileName[BUFSIZE];

	speed =100;
	dir = 0;
	initializeBoard();
/*
*/


	time(&rawtime);
    timeinfo = localtime(&rawtime);

    strcpy(appender,"W_SCN");
    getFileName(appender,fileName);

    FILE *fp;
    fp = fopen(fileName,"w");
    if (!fp){
        printf("Unable to open file %s\n",fileName);
        fflush(stdout);
        exit(1);
    }
	printf("%s\n",fileName);

	setRS485StepperMotorSpeed(0xA0,speed);
// move motor back 50 steps
	dir=0;
	steps = 25;
	moveRS485StepperMotor(0xA0,steps,dir);
	delay(500);


	dir = 1;
	fprintf(fp,"Step,Encoder\n");
	for (i=0;i<50;i++){
		moveRS485StepperMotor(0xA0,1,dir);
		delay(500);
		getRS485DigitalOut(0xA6,&position);
		fprintf(fp,"%i,%i\n",i-25,position);
	    printf("%i\t%i\n",i-25,position);
	}
// move it back
	steps = 25;
	dir = 0;
	moveRS485StepperMotor(0xA0,steps,dir);
    delay(100);
    fclose(fp);


printf("\nOK\n");
return 0 ;
}

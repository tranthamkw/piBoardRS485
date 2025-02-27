/*
 * This is a template file to show all of the necessary components to add

test digital in

 * to new data collection files that are created. 
*/
#include <time.h>
#include "piBoard.h"
#include "RS485Devices.h"
#include "myFileIO.h"

int main (int argc, char* argv[]){
	unsigned short speed;
	time_t rawtime, now;
    double seconds;
    struct tm * timeinfo;
	char appender[BUFSIZE],fileName[BUFSIZE];
	
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

time(&rawtime);
    timeinfo = localtime(&rawtime);

    strcpy(appender,"W_IDX");
    getFileName(appender,fileName);

    FILE *fp;
    fp = fopen(fileName,"w");
    if (!fp){
        printf("Unable to open file %s\n",fileName);
        fflush(stdout);
        exit(1);
    }


	getRS485DigitalOut(0xA6,&speed);

 	fprintf(fp,"%s\n",fileName);
    fprintf(fp,"%i\n",speed);

	printf("%i\n",speed);

	fclose(fp);

	delay(100);

printf("\nOK\n");
return 0 ;
}

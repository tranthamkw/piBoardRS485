
/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include "piBoard.h"
#include "RS485Devices.h"
#include "myFileIO.h"


int main (int argc, char* argv[]){
	int i,k,runtime;

	double seconds;

	char appender[BUFSIZE];
	char fileName[BUFSIZE];

	if (argc==2) {
		strcpy(appender,argv[1]);
	} else {
		printf("tester\n");
		strcpy(appender,"tester");

	}


	FILE *fp;
	initializeBoard();

	getFileName(appender,fileName);

	fp = fopen(fileName,"w");
	if (!fp){
		printf("Unable to open file %s\n",fileName);
		fflush(stdout);
		exit(1);
	}
	fprintf(fp,"%s\n",fileName);

	fprintf(fp,"time(s),PV1,SV1,PV2,SV2,PV3,PV4,AnalogADCCount\n");

	fclose(fp);

printf("ok\n");


	return 0 ;
}

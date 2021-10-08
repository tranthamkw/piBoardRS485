/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include "piBoard.h"
#include "RS485Devices.h"
#include "SRSLockIn.h"
#include "omegaCN7500.h"
#include "myFileIO.h"

// THESE ARE THE RS485 ADDRESSES OF EACH RS485 INSTRUMENT/BRIDGE/SLAVE DEVICE
#define SRS830 0xCA
#define SRS530 0xC6
#define OMEGA 8//omegaCN7800


int main (int argc, char* argv[]){



	int i,k,runtime,numloops,pausetime;
	float myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT;
	unsigned int periods;

	time_t rawtime, now;
	double seconds;
	struct tm * timeinfo;

	char appender[BUFSIZE],fileName[BUFSIZE];
	char comments[512];


	if (argc==4){
		numloops = atoi(argv[1]);
		pausetime = atoi(argv[2]);
		strcpy(comments,argv[3]);
	}else{
		printf("Usage '~$sudo ./ recordSRS <int:numloops> <int:mS to pause between aqc> <comments in quotes>'\n");
		return 1;
	}


	initializeBoard();
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strcpy(appender,"recordSRS");
	getFileName(appender,fileName);

	FILE *fp;
	fp = fopen(fileName,"w");
	if (!fp){
		printf("Unable to open file %s\n",fileName);
		fflush(stdout);
		exit(1);
	}

	printf("%s\n",fileName);
	fprintf(fp,"%s\n",fileName);
	fprintf(fp,"%s\n",comments);
	fprintf(fp,"seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT\n");

	printf("initializing SRS530 . . .\n");
	initSRS530(SRS530);
	printf("initializing SRS830 . . .\n");
	initSRS830(SRS830);

	k=0;

	do {
		time(&now);
		seconds = difftime(now,rawtime);

		i = getPVCN7500(OMEGA,&myT);
		getSRS830Data(&myR8,&myPhi8,&myf8,SRS830);
		getSRS530R(&myR5,SRS530);
		getSRS530Phi(&myPhi5,SRS530);
		getSRS530Freq(&myf5,SRS530);


		printf("%.f\t%e\t%.3f\t%.3f\t%.3e\t%.3f\t%.3f\t%.1f\n", seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT);

		fprintf(fp,"%.f,%e,%.3f,%.3f,%.3e,%.3f,%.3f,%.1f\n", seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT);
	delay(pausetime);


	k++;
	} while (k<numloops);

	fclose(fp);

	return 0 ;
}

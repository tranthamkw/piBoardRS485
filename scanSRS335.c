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
#define SRS335 0xC0
#define OMEGA 8//omegaCN7800


int main (int argc, char* argv[]){



	int i,k,runtime,numloops,pausetime;
	float myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT;
	float myf335,myA335;
	float beginf,endf,deltaf,setf;
	unsigned int periods;

	time_t rawtime, now;
	double seconds;
	struct tm * timeinfo;

	char appender[BUFSIZE],fileName[BUFSIZE];
	char comments[512];


	if (argc==6){
		beginf = atof(argv[1]);
		endf = atof(argv[2]);
		deltaf = atof(argv[3]);
		pausetime = atoi(argv[4]);
		strcpy(comments,argv[5]);

	}else{
		printf("Usage '~$sudo ./ scanSRS <f begin> <f end> <delta f> <int:mS to pause between aqc> <comments in quotes>'\n");
		return 1;
	}
	printf("Begin frequency %f\n\n",beginf);
	printf("End frequency %f\n\n",endf);
	printf("Frequency step size %f\n\n",deltaf);

	numloops = (int)((endf - beginf)/deltaf)+2;

	if (numloops<0){
		printf("Error: end frequency greater than begin frequency\n");
		return 1;
	}

	printf("Number of loops = %d\n\n",numloops);
	printf("Estimated time to completion = %d seconds = %.2f minutes\n\n",(numloops*(pausetime+2100))/1000,((float)(numloops*(pausetime+2100))/60000.0));

	initializeBoard();
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strcpy(appender,"scanSRS");
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
	fprintf(fp,"seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myf335,myA335,myT\n");

	printf("initializing SRS530 . . .\n");
	initSRS530(SRS530);
	printf("initializing SRS830 . . .\n");
	initSRS830(SRS830);

	printf("seconds\tmyR8\t\tmyPhi8\tmyf8\tmyR5\t\tmyPhi5\tmyf5\tmyf335\tmyA335\tmyT\n");


	k=0;

	do {
		time(&now);
		seconds = difftime(now,rawtime);
		setf = (beginf + (float)k * deltaf);

		setSRS335Freq(setf,SRS335);
		delay(pausetime);

		i = getPVCN7500(OMEGA,&myT);
		getSRS830Data(&myR8,&myPhi8,&myf8,SRS830);
		getSRS530R(&myR5,SRS530);
		getSRS530Phi(&myPhi5,SRS530);
		getSRS530Freq(&myf5,SRS530);
		getSRS335Freq(&myf335,SRS335);
		getSRS335Ampl(&myA335,SRS335);


		printf("%.f\t%e\t%.3f\t%.3f\t%.3e\t%.3f\t%.3f\t%.3f\t%.3f\t%.1f\n", seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myf335,myA335,myT);

		fprintf(fp,"%.f,%e,%.3f,%.3f,%.3e,%.3f,%.3f,%.3f,%.3f,%.1f\n", seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myf335,myA335,myT);


	k++;
	} while (k<numloops);

	fclose(fp);

	return 0 ;
}

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
#define SSR 0xA1
#define ON 0x01
#define OFF 0x00

int main (int argc, char* argv[]){

	int i,k,runTime,powerTime,waitTime;
	float myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT;
	unsigned int periods;

	float Thi,Tlo;
	time_t rawtime,now2,now,startPower,startWait;
	double seconds,dt;
	struct tm * timeinfo;

	char appender[BUFSIZE],fileName[BUFSIZE];
	char comments[512];

	if (argc==7){
		powerTime = atoi(argv[1]);
		waitTime = atoi(argv[2]);
		runTime = atoi(argv[3]);
		Thi = atof(argv[4]);
		Tlo = atof(argv[5]);
		strcpy(comments,argv[6]);
	}else{
		printf("Usage '~$sudo ./ tempertureScan <int:PowerTimeSec> <int:WaitTimeSec> <int:MaxTotalTime> <float:Thi> <float:Tlo> <Comments in quotes>'\n");
		exit(1);
	}


/* 
	runtime = max runtime
	Thi = maximum temperature
	Tlo = minium temperature

	heats/or/cools for  powerTime seconds
	Wait waitTime seconds

	record parameters. 10 iterations.

	repeat until myT>Thi or myT<Tlo
	escape if time exceeds  runtime.

	*/


	initializeBoard();
	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strcpy(appender,"tempScan");
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
		// get data
		printf("\n\n");
		printf("seconds\tmyR8\tmyPhi8\tmyf8\tmyR5\tmyPhi5\tmyf5\tmyT\n");

		for (k=0;k<10;k++){
		time(&now2);
		seconds = difftime(now2,rawtime);
		getSRS830Data(&myR8,&myPhi8,&myf8,SRS830);
		getSRS530R(&myR5,SRS530);
		getSRS530Phi(&myPhi5,SRS530);
		getSRS530Freq(&myf5,SRS530);
		getPVCN7500(OMEGA,&myT);
		printf("%.f\t%e\t%.3f\t%.3f\t%.3e\t%.3f\t%.3f\t%.1f\n", seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT);
		fprintf(fp,"%.f,%e,%.3f,%.3f,%.3e,%.3f,%.3f,%.1f\n", seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT);
		delay(500);}


	do {
		time(&now2);


		// turn on heat/cool
		
		time(&startPower);
		printf("Actively heating or cooling\n");
		printf("Remaining(s)\tTemperature(C)\n");
		setRS485DigitalOut(SSR,ON);
		do{
		time(&now);
		// wait 
		// print wait time and temperature while waiting
			dt= difftime(now,startPower);
			getPVCN7500(OMEGA,&myT);
			printf("                           \r");
			printf("\t%.f\t%.2f\t\t\r",(powerTime-dt),myT);
			fflush(stdout);
			delay(1000);
		}while ((dt < powerTime)&&(myT < Thi)&&(myT > Tlo));


		// turn off heat/cool

		setRS485DigitalOut(SSR,OFF);
		time(&startWait);
		printf("\nWaiting for temperature to thermalize\n");
		printf("Remaining(s)\tTemperature(C)\n");
		do{
		time(&now);
		// wait 
		// print wait time and temperature while waiting
			dt= difftime(now,startWait);
			getPVCN7500(OMEGA,&myT);
			printf("                           \r");
			printf("\t%.f  \t%.2f\t\t\r",(waitTime-dt),myT);
			fflush(stdout);
			delay(1000);
		}while ((dt < waitTime));

		// get data
		printf("\n\n");
		printf("seconds\tmyR8\tmyPhi8\tmyf8\tmyR5\tmyPhi5\tmyf5\tmyT\n");

		for (k=0;k<10;k++){
		time(&now2);
		seconds = difftime(now2,rawtime);
		getSRS830Data(&myR8,&myPhi8,&myf8,SRS830);
		getSRS530R(&myR5,SRS530);
		getSRS530Phi(&myPhi5,SRS530);
		getSRS530Freq(&myf5,SRS530);
		getPVCN7500(OMEGA,&myT);
		printf("%.f\t%e\t%.3f\t%.3f\t%.3e\t%.3f\t%.3f\t%.1f\n", seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT);
		fprintf(fp,"%.f,%e,%.3f,%.3f,%.3e,%.3f,%.3f,%.1f\n", seconds,myR8,myPhi8,myf8,myR5,myPhi5,myf5,myT);
		delay(500);
		}

	} while ((seconds < runTime)&&(myT < Thi)&&(myT > Tlo));

	setRS485DigitalOut(SSR,OFF);

	fclose(fp);

	return 0 ;
}

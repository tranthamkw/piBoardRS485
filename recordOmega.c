
/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include "piBoard.h"
#include "RS485Devices.h"
#include "omegaCN7500.h"
#include "omegaCN740.h"

// THESE ARE THE RS485 ADDRESSES OF EACH RS485 INSTRUMENT/BRIDGE/SLAVE DEVICE
#define OMEGA3 12 // omegaCN740 pv3
#define OMEGA1 8 //omegaCN7800 sv1,pv1
#define OMEGA2 6 //omegaCN7800 sv2,pv2
#define OMEGA4 14 //omegaCN740 pv4
#define ANALOG 0xA1
#define BUFSIZE 64



int main (int argc, char* argv[]){
	int i,k,runtime;
	float myTemp,myVolts, pv1, sv1, pv2, sv2, pv3, pv4,volts,stdev;

	time_t rawtime, now;
	double seconds;
	struct tm * timeinfo;
	char filePath[BUFSIZE], fileName[BUFSIZE], buffer[BUFSIZE];
	char appender[BUFSIZE];

	if (argc==2) {
		runtime = atoi(argv[1]);
	} else {
		runtime = 30;
		printf("Using default runtime of 30 seconds\n");
	}


	appender = "tester";

	FILE *fp;
	initializeBoard();
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(filePath,BUFSIZE,"/home/pi/data/%F",timeinfo);
	if (stat(filePath, &st) == -1){
		mkdir(filePath, S_IRWXU | S_IRWXG | S_IRWXO);
	printf("%s",filePath);
	}
	sprintf(buffer,"%s_%F_%H%M%S",appender);
	strftime(fileName,BUFSIZE,buffer,timeinfo);
	sprintf(buffer,"%s.csv",fileName);
	printf("\n%s\n",buffer);
	sprintf(fileName,"%s/%s",filePath,buffer);


	fp = fopen(fileName,"w");
	if (!fp){
		printf("Unable to open file %s\n",fileName);
		fflush(stdout);
		exit(1);
	}

	fprintf(fp,"time(s),PV1,SV1,PV2,SV2,PV3,PV4,AnalogADCCount\n");

do {

	time(&now);
	seconds = difftime(now,rawtime);

	i=readRS485AnalogRecorder(ANALOG,0,5.0,&volts,&stdev);
	delay(130);

	i=getPVCN7500(OMEGA1,&myTemp);
	if (!i) pv1=myTemp;
	delay(130);

	i=getPVCN7500(OMEGA2,&myTemp);
	if (!i) pv2=myTemp;
	delay(130);

	i=getSVCN7500(OMEGA1,&myTemp);
	if(!i) sv1 = myTemp;
	delay(130);

	i=getSVCN7500(OMEGA2,&myTemp);
	if(!i) sv2 = myTemp;
	delay(130);

	i=getPVCN740(OMEGA3,&myTemp);
	if (!i)	pv3=myTemp;
	delay(130);

	i=getPVCN740(OMEGA4,&myTemp);
	if (!i)	pv4=myTemp;

	printf("%.f\t%.1f\t%.1f\t%.1f\t%.1f\t%.1f\t%.1f\t%.3f\n",seconds,pv1,sv1,pv2,sv2,pv3,pv4,volts);
	fprintf(fp,"%.f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.3f\n",seconds,pv1,sv1,pv2,sv2,pv3,pv4,volts);

	delay(2000);
} while (seconds<runtime);

fclose(fp);

	return 0 ;
}

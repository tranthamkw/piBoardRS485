
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
#define OMEGA1 8 //omegaCN7800 sv1,pv1



int main (int argc, char* argv[]){
	int i,k,runtime;
	float myTemp,myVolts, pv1, sv1, pv2, sv2, pv3, pv4,volts,stdev;
	time_t now = time(NULL);
	double seconds;
	char time_str[50];
	char filename[128];

	FILE *fp;
	initializeBoard();

	struct tm *local_time = localtime(&now);

	sprintf(filename,"/home/pi/data/logOmega.csv");

	fp = fopen(filename,"a");
	if (!fp){
		printf("Unable to open file %s\n",filename);
		fflush(stdout);
		exit(1);
	}


	strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);
	
	i=getPVCN7500(OMEGA1,&myTemp);
	
	if (!i) pv1=myTemp;
	delay(130);


	printf("%s\t%.1f\n",time_str,pv1);
	fprintf(fp,"%s,%.1f\n",time_str,pv1);

	delay(20);

	fclose(fp);

	return 0 ;
}

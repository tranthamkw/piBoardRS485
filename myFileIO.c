
/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/

#include "myFileIO.h"

int getFileName(char* app,char* fileName){
	time_t rawtime, now;
	struct tm * timeinfo;
	char filePath[BUFSIZE], buffer[BUFSIZE];
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	struct stat st = {0};
	strftime(filePath,BUFSIZE,"/home/pi/data/%F",timeinfo);
	if (stat(filePath, &st) == -1){
		mkdir(filePath, S_IRWXU | S_IRWXG | S_IRWXO);
//debug 	printf("FILEPATH: %s",filePath);
	}
	strftime(fileName,BUFSIZE,"%F_%H%M%S",timeinfo);
	sprintf(buffer,"%s.csv",fileName);
//debug 	printf("BUFFER  \n%s\n",buffer);
	sprintf(fileName,"%s/%s%s",filePath,app,buffer);
//debug 	printf("File name  %s\n",fileName);
}

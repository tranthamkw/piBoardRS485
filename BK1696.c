/*


 */

#include "BK1696.h"



int initializeBK1696(unsigned short address){
	unsigned char chardata[64];
	unsigned char command[16];
	int j;

	strncpy((char*)command,"SESS00",sizeof(command));

	writeRS485to232Bridge(command,chardata,address);
	j=-1;
	//j=strncmp(chardata,"OK",2);
	if (strstr((char*)chardata,"OK") != NULL){ // found "OK"
		j=0;
	}else{
	printf("BK1696 error\n");
	printf("Sent command %s\n",command);
	printf("Return string %s\n",chardata);
	}
	
return j;
}

int setOutputBK1696(unsigned short address,int status){
	unsigned char chardata[64];
	unsigned char command[16];
	unsigned char add[5];
	int j;

	strncpy((char*)command,"SOUT00",sizeof(command));
	
	if (status==0) {
	add[0]=48;
	} else {
	add[0]=49;
	}
		add[1]='\0';
	strcat((char*)command,(char*)add);
	writeRS485to232Bridge(command,chardata,address);
	j=-1;
	//j=strncmp(chardata,"OK",2);
	if (strstr((char*)chardata,"OK") != NULL){ // found "OK"
		j=0;
		}else{
	printf("BK1696 error\n");
	printf("Sent command %s\n",command);
	printf("Return string %s\n",chardata);

	}

return j;
}



int getVoltsAmpsBK1696(unsigned short address, float* volts, float* amps){
	unsigned char chardata[64];
	unsigned char command[16]="GETD00";
	int j,i;
	float tempv;


	writeRS485to232Bridge(command,chardata,address);
	
	j=-1;
	if (strstr((char*)chardata,"OK") != NULL){
		for (i=0;i<4;i++){
			command[i]=chardata[i];
		}
		command[i]='\0';
		tempv = atof((char*)command);
		*volts = tempv/100.0;


		for (i=0;i<4;i++){
			command[i]=chardata[i+4];
		}
		command[i]='\0';
		tempv = atof((char*)command);
		*amps = tempv/1000.0;
	 j=0;
	}else{
	printf("BK1696 error\n");
	printf("Sent command %s\n",command);
	printf("Return string %s\n",chardata);
	}
return j;	
}


int setVoltsBK1696(unsigned short address, float volts){
	unsigned char chardata[64];
	unsigned char command[16]="VOLT00";
	char s[5];
	int j,i;

if (volts>20.0) volts=20.0;
if (volts<0.0) volts=0.0;

	j = lroundf(volts*10.0);
//printf("j= %d\t",j);
	i = j/100;
//printf("i= %d\t",i);
	s[0] = 48+i;
	j = j-(i*100);
	i = j/10;
//printf("i= %d\t",i);
	s[1] = 48+i;
	i = j%10;
//printf("i= %d\n",i);
	s[2] = 48+i;

	s[3] = 13;
	s[4] = '\0';
	strcat((char*)command,s);

//	printf("%s\n",command);

	writeRS485to232Bridge(command,chardata,address);

	j=-1;
	//j=strncmp(chardata,"OK",2);
	if (strstr((char*)chardata,"OK") != NULL){ // found "OK"
		j=0;
	}else{
	printf("BK1696 error\n");
	printf("Sent command %s\n",command);
	printf("Return string %s\n",chardata);
	}
return j;
}


int setAmpsBK1696(unsigned short address, float amps){
	unsigned char chardata[64];
	unsigned char command[16]="CURR00";
	char s[5];
	int j,i;

if (amps>10.0) amps=10.0;
if (amps<0.0) amps=0.0;

	j = lroundf(amps*100.0);
printf("j= %d\t",j);
	i = j/100;
printf("i= %d\t",i);
	s[0] = 48+i;
	j = j-(i*100);
	i = j/10;
printf("i= %d\t",i);
	s[1] = 48+i;
	i = j%10;
printf("i= %d\n",i);
	s[2] = 48+i;

	s[3] = 13;
	s[4] = '\0';
	

	strcat((char*)command,s);

	printf("%s\n",command);

	writeRS485to232Bridge(command,chardata,address);

	j=-1;
	//j=strncmp(chardata,"OK",2);
	if (strstr((char*)chardata,"OK") != NULL){ // found "OK"
		j=0;
	}else{
	printf("BK1696 error\n");
	printf("Sent command %s\n",command);
	printf("Return string %s\n",chardata);

	}

return j;




}

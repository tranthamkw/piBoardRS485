/*



*/

#include "omegaCN740.h"

//these are register definitions specific to the Omega CN7800
#define PV_REG 0x4700
#define SV_REG 0x4701

int getPVCN740(unsigned short chan, float* temperature){
	unsigned int returndata;
	int status;
//	initialize_rs485(9600,25);
	status=read_Modbus_RTU(chan,PV_REG,&returndata); //register4096 is PV

	if(status==0){
		*temperature =(float)returndata/10.0;
	}else{
		printf("Channel %d  error reading CN740 code %d\n",chan, status);
	}
	return status;
}

int getSVCN740(unsigned short chan, float* temperature){
	unsigned int returndata;
	int status;
//	initialize_rs485(9600,25);

	status=read_Modbus_RTU(chan,SV_REG,&returndata); //register4096 is SV
	if(status==0){
		*temperature  =(float)returndata/10.0;
	}else{
		printf("Channel %d  error reading CN740 code %d\n",chan, status);
	}
	return status;
}

int setSVCN740(unsigned short chan, float temperature){
	int status;
//	initialize_rs485(9600,25);

	status=write_Modbus_RTU(chan,SV_REG, (unsigned int) (temperature*10));

	return status;
}

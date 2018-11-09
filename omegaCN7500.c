/*



*/

#include "omegaCN7500.h"

//these are register definitions specific to the Omega CN7800
#define PV_REG 0x1000
#define SV_REG 0x1001

int getPVCN7500(unsigned short chan, float* temperature){
	unsigned int returndata;
	int status;
//	initialize_rs485(9600,25);
	status=read_Modbus_RTU(chan,PV_REG,&returndata); //register4096 is PV

	if(status==0){
		*temperature =(float)returndata/10.0;
	}else{
		printf("error reading CN7500 code %d\n",status);
	}
	return status;
}

int getSVCN7500(unsigned short chan, float* temperature){
	unsigned int returndata;
	int status;
//	initialize_rs485(9600,25);

	status=read_Modbus_RTU(chan,SV_REG,&returndata); //register4096 is SV
	if(status==0){
		*temperature  =(float)returndata/10.0;
	}else{
		printf("error reading CN7500 code %d\n",status);
	}
	return status;
}

int setSVCN7500(unsigned short chan, float temperature){
	int status;
//	initialize_rs485(9600,25);

	status=write_Modbus_RTU(chan,SV_REG, (unsigned int) (temperature*10));

	return status;
}


#include "RS485Devices.h"
/*
	This is the interface between the controlling code and the hardware. This provides proper formating and register
	information for each function. General header File to encapsulate all rs485 slave device functions
*/

#include "piBoard.h"
#include <math.h>


void printHexData(unsigned char* buff, unsigned short len){
	unsigned short i;
	for (i=0;i<len;i++){
		printf("%02x  ", buff[i]);
	}
printf("\n");
}

int changeRS485Address(unsigned short Address, unsigned short newAddress){
	unsigned char outData[16],rtnData[16];
	    unsigned short temp,bytes_to_write;
	unsigned int k;
	int status;
	outData[0] = (unsigned char) (Address & 0x00FF);	
	outData[1] = 0x06;
	outData[2] = 0x00;
	outData[3] = 0xF0;
	outData[4] = Address & 0x00FF;
	outData[5] = newAddress & 0x00FF;

	// calculate the CRC
	temp=modRTU_CRC(outData,6);
	// append the CRC in the correct order
	outData[7]=(temp & 0xFF00)>>8;
	outData[6]=(temp & 0x00FF);
	// establish the correct number of bytes to send out
	bytes_to_write=8;
		//printf("out:\t");printHexData(outData,bytes_to_write);
	write_rs485BYTE(outData,bytes_to_write,rtnData,&k);
		//printf("rtn:\t");printHexData(rtnData,k);
	status=0;
	if (rtnData[1] & 0x80){
		status = (rtnData[2]<<8) | rtnData[3];
		//debuging information
		printHexData(rtnData,k);
	}
return status;
}


int getRS485SlaveID(unsigned char* returnData, unsigned  short Address){

	/*

	*/
    int i, status;
    unsigned char outData[64];
    unsigned char rtnData[64];
	unsigned int k;
    unsigned short temp,bytes_to_write;

	outData[0] = (unsigned char) (Address & 0x00FF);	
	outData[1] = 0x03;
	outData[2] = 0x00;
	outData[3] = 0xF0;
	// calculate the CRC
	temp=modRTU_CRC(outData,4);
	// append the CRC in the correct order
	outData[5]=(temp & 0xFF00)>>8;
	outData[4]=(temp & 0x00FF);
	// establish the correct number of bytes to send out
	bytes_to_write=6;
		//printf("out:\t");printHexData(outData,bytes_to_write);
	write_rs485BYTE(outData,bytes_to_write,rtnData,&k);
		//printf("rtn:\t");printHexData(rtnData,k);
	status=0;
	if (k>0){
	if (!(rtnData[1] & 0x80)){
		if ((rtnData[2]+5)==k) {
			for (i=0;i<rtnData[2];i++){
				returnData[i]=rtnData[i+3];
			}
			returnData[i]=0;
			} else {
				status = 1;
				printf("getSlaveID process returned an unexpected number of bytes\n");
				printHexData(rtnData,k);
				printf("%s\n",rtnData);
				returnData[0]=0;
				}
		} else {
		status=rtnData[2]<<8|rtnData[3];
		returnData[0]=0;
		printf("getSlaveID process returned error code %04x \n",temp);
		}
	} else {
	status = 1;
	returnData[0]=0;
	printf("No Response from RS485 device at address %02X=%d \n",outData[0],outData[0]);
	}
		//printf("%s\n",returnData);
return status;

}

int moveRS485StepperMotor(unsigned short chan, unsigned short steps, unsigned char dir){
	int status;
	unsigned short command;
	command = steps;
	status=write_Modbus_RTU(chan,BASEREGSTEPMTR, command);
	return status;
}

int getRS485StepperMotorSteps(unsigned short chan, unsigned int* steps){
	unsigned int temp;
	int status;
	status = read_Modbus_RTU(chan,BASEREGSTEPMTR, &temp);
	*steps = temp;
	return status;
}

int setRS485StepperMotorSpeed(unsigned short chan, unsigned short speed){
	int status;
	if ((speed>49)&(speed<251)){
	status=write_Modbus_RTU(chan,BASEREGSTEPMTR+4, speed);
	} else status=1;
	return status;
}

int getRS485StepperMotorSpeed(unsigned short chan, unsigned int* speed){
	int status;
	unsigned int temp;
	status = read_Modbus_RTU(chan,BASEREGSTEPMTR+4, &temp);
	*speed = temp;
	return status;
}

int setRS485StepperMotorStepsRev(unsigned short chan, unsigned short stepsperrev){
// used internally by the PIC to check for homing errors.
	int status;
	if (stepsperrev>0) {
	status=write_Modbus_RTU(chan,BASEREGSTEPMTR+1, stepsperrev);
	} else status=1;
	return status;
}


int getRS485StepperMotorStepsRev(unsigned short chan, unsigned int* stepsperrev){
// used internally by the PIC to check for homing errors. not used for anything else
	int status;
	unsigned int temp;
	status = read_Modbus_RTU(chan,BASEREGSTEPMTR+1, &temp);
	*stepsperrev = temp;
	return status;
}


int findRS485HomeStepperMotor(unsigned short chan, unsigned char state, unsigned char direction){
	// state is the state the pic is looking for, direction is the direction to move the motor
	// state = 0 or 1
	// direction = 0 or 1
	int status;
	unsigned short command;
	command = 0x0001 | ((state&0x0001) <<1) | ((direction&0x0001)<<2);
	//printf("%d \n",command);
	status=write_Modbus_RTU(chan,BASEREGSTEPMTR+16, command);
	return status;
}


int setRS485ServoPosition(unsigned short chan, unsigned short servo, unsigned short position){
	int status;
	if (position > 10) position = 10;
	if (servo>1) servo=1;
	status=write_Modbus_RTU(chan,BASEREGSERVO+servo, position);
	return status;
}

int getRS485ServoPosition(unsigned short chan, unsigned short servo, unsigned int* position){
	int status;
	unsigned int temp;
	if (servo>1) servo=1;
	status = read_Modbus_RTU(chan,BASEREGSERVO+servo, &temp);
	*position = temp;
	return status;
}

int setRS485DigitalOut(unsigned short chan, unsigned short value){
/*
	two outputs: Out1 = value & 0x01, Out2 = (value & 0x02) >>1
	value = bit8
		bit7
		...
		bit3
		bit2 = Out2
		bit1 = Out1
*/
	int status;
	status = write_Modbus_RTU(chan,BASEREGSERVO+16,value);
	return status;
}

int getRS485DigitalOut(unsigned short chan, unsigned short* value){
	int status;
	unsigned int temp;
	status = read_Modbus_RTU(chan, BASEREGSERVO+16,&temp);
	*value=(unsigned short) temp;
	return status;
}


int setRS485AnalogRecorderPeriod(unsigned short RS485Chan, unsigned short ADCperiods){
	int status;
	if (ADCperiods>100) ADCperiods=100;
	if (ADCperiods<1) ADCperiods=1;
	status = write_Modbus_RTU(RS485Chan,BASEREGANLG+16,ADCperiods);
	return status;
}

int getRS485AnalogRecorderPeriod(unsigned short RS485Chan, unsigned int* ADCperiods){
	int status;
	unsigned int temp;
	status = read_Modbus_RTU(RS485Chan,BASEREGANLG+16,&temp);
	*ADCperiods=temp;
	return status;
}

unsigned int readRS485AnalogRecorder(unsigned short RS485Chan, unsigned short AnalogChan, float vref, float* volts, float* stdev){
	unsigned char returndata[64];
	unsigned char cmd[16];  
	unsigned int i,j;
	unsigned int temp;
	float tempf;
		cmd[0]=RS485Chan;
		cmd[1]=0x08;  // This command code is unique to the analog recorder. Expected return is 4 bytes. for these
				// reasons we cannot use "read_Modbus_RTU" function
		cmd[2]=(BASEREGANLG & 0xFF00)>>8;
		cmd[3]=(BASEREGANLG & 0x00FF)+AnalogChan; //channel number.  0 to 3
		i=4;
		temp = modRTU_CRC(cmd,i);
		cmd[i+1]=(unsigned char)((temp&0xFF00)>>8);  //ensures that the MSByte is sent 
		cmd[i]=(unsigned char)(temp&0x00FF);  //before the LSByte
		write_rs485BYTE(cmd,i+2, returndata, &j);  // this is a generic write to the RS485 bus from the PI. 

	if (!(returndata[1] & 0x80)){
		temp = ((returndata[3] << 8)| returndata[4]);  
//		printf("Raw sum of 7 measurements (0 to 1024*7) = %d\n",temp);
		tempf = ((float) temp)/7.0;
		*volts = tempf*vref/1024.0;
		temp = (( returndata[5] <<8)| returndata[6]);
//		printf("Raw sum of deviations: (7*Xi - 7*Xavg) = %d\n",temp);
		tempf =((float) temp) / 49.0;
		tempf = sqrt(tempf/6.0);  // should this be stderror instead of stdev?
		*stdev=tempf*vref/1024.0;
		temp=0;
	} else {
		temp=returndata[2]<<8|returndata[3];
		printf("process retunred erro code %04x \n",temp);
	}
return temp;
}

unsigned int readRS485AnalogRecorderSimple(unsigned short RS485Chan, unsigned short AnalogChan, float* ADCcount){
	int status;
	unsigned int temp;
	if (AnalogChan>3) AnalogChan=3;
	status = read_Modbus_RTU(RS485Chan,BASEREGANLG+AnalogChan,&temp);
	*ADCcount=(float)temp;
	return status;
}

int writeRS485to232Bridge(unsigned char* cmd, unsigned char* returnData, unsigned  short Address){

/*

Writes an ascii command to a 485-232 bridge device. this automatically appends a CR to data,
which is a very common terminator character with RS232 ascii communications. 
The bridge device is addressed, and data is written to a register.  The bridge strips this, and and CRC
portions and passes on to RS232.
*/
    int i,length,j, status;
	unsigned char outData[64];
	unsigned char rtnData[64];
	unsigned int k;
	unsigned short temp,bytes_to_write;

	outData[0] = (unsigned char) (Address & 0x00FF);
	outData[1] = 0x06;
	outData[2] = (unsigned char)(((BASEREG485BRIDGE232+32) & 0xFF00)>>8);
	outData[3] = (unsigned char)((BASEREG485BRIDGE232+32)&0x00FF);
	length = strlen((char*)cmd);
	for (j=0;j<length;j++){
			outData[j+4]=cmd[j];
	}
	outData[length+4]=13; //append a CR
	// calculate the CRC
	temp=modRTU_CRC(outData,length+5);
	// append the CRC in the correct order
	outData[length+6]=(temp & 0xFF00)>>8;
	outData[length+5]=(temp & 0x00FF);
	// establish the correct number of bytes to send out
	bytes_to_write=length+7;
//		printf("out:\t");printHexData(outData,bytes_to_write);
	write_rs485BYTE(outData,bytes_to_write,rtnData,&k);
//		printf("rtn:\t");printHexData(rtnData,k);
	status=0;
	if(k>0){
	if (!(rtnData[1] & 0x80)){
		if ((rtnData[2]+5)==k) {
			for (i=0;i<rtnData[2];i++){
				returnData[i]=rtnData[i+3];
			}
			returnData[i]=0;
			} else {
				status = 1;
				printf("writeRS232Bridge process returned an unexpected number of bytes\n");
				printHexData(rtnData,k);
				printf("%s\n",rtnData);
				returnData[0]=0;
				}
		} else {
		status=rtnData[2]<<8|rtnData[3];
		returnData[0]=0;
		printf("writeRS232Bridge process returned error code %04x \n",temp);
	}
	} else {
	status=1;
	returnData[0]=0;
	printf("No Response from RS232 bridge at address %02X=%d \n",outData[0],outData[0]);
	}
		//printf("%s\n",returnData);
return status;
}


int setRS485BridgeReads(unsigned short reads, unsigned short Address){
	int status;
	if (reads>10) reads=10;
	if (reads<1) reads=1;
	status = write_Modbus_RTU(Address,BASEREG485BRIDGE232+1,reads);
	return status;
}

int getRS485BridgeReads(unsigned short* reads, unsigned short Address){
	int status;
	unsigned int temp;
	status = read_Modbus_RTU(Address,BASEREG485BRIDGE232+1, &temp);
	*reads = temp;
	return status;
}

int setRS485BridgeTimeout(unsigned short timeout, unsigned short Address){
	int status;
	status = write_Modbus_RTU(Address,BASEREG485BRIDGE232+2,timeout);
	return status;

}

int getRS485BridgeTimeout(unsigned short* timeout, unsigned short Address){
	int status;
	unsigned int temp;
	status = read_Modbus_RTU(Address,BASEREG485BRIDGE232+2, &temp);
	*timeout = temp;
	return status;
}

int setRS485BridgeDebugPrint(unsigned short debug, unsigned short Address){
	int status;
	status = write_Modbus_RTU(Address,BASEREG485BRIDGE232+4,debug);
	return status;
}


int getRS485GPIBStatus(unsigned short* GPIBStatus, unsigned short Address){
	int status;
	unsigned int temp;
	status = read_Modbus_RTU(Address,BASEREG485BRIDGE232+3, &temp);
	*GPIBStatus = temp;
	return status;
}

int resetGPIBBridge(unsigned short Address){
	int status;
	status = write_Modbus_RTU(Address,BASEREG485BRIDGE232+3,0x00);
	return status;
}


int sendGPIBCommand(unsigned char* cmd, unsigned short Address){
// see www.ni.com/white-paper/3389/en/  table 1 for full list of commands.

	 int length,j, status;
	unsigned char outData[64];
	unsigned char rtnData[64];
	unsigned int k;
	unsigned short temp,bytes_to_write;

	outData[0] = (unsigned char) (Address & 0x00FF);
	outData[1] = 0x06;
	outData[2] = (unsigned char)(((BASEREG485BRIDGE232+16) & 0xFF00)>>8);
	outData[3] = (unsigned char)((BASEREG485BRIDGE232+16)&0x00FF);
	length = strlen((char*)cmd);
	for (j=0;j<length;j++){
			outData[j+4]=cmd[j];
	}
//	printf(">>>>Strlen %d\n",length);
//	outData[length+4]=13; //append a CR
	// calculate the CRC
	temp=modRTU_CRC(outData,length+5);
	// append the CRC in the correct order
	outData[length+5]=(temp & 0xFF00)>>8;
	outData[length+4]=(temp & 0x00FF);
	// establish the correct number of bytes to send out
	bytes_to_write=length+6;
//		printf("out:\t");printHexData(outData,bytes_to_write);
	write_rs485BYTE(outData,bytes_to_write,rtnData,&k);
//		printf("rtn:\t");printHexData(rtnData,k);
	status=0;
	if (rtnData[1] & 0x80){
		status = (rtnData[2]<<8) | rtnData[3];
		printf("sendGPIBCommand %s return error code %d\n",cmd,status);
		printHexData(rtnData,k);

	}
return status;
}


int closeGPIBBridge(unsigned short Address){
	unsigned char command[4];
	int status;
	command[0]=0x01;
	command[1]=0x00;

	status=sendGPIBCommand(command,Address);
	return status;

}

int sendGPIBData(unsigned char *cmd, char gpibaddress, unsigned short Address){
	int length,j, status;
	unsigned char outData[64];
	unsigned char rtnData[64];
	unsigned int k;
	unsigned short temp,bytes_to_write;

	outData[0] = (unsigned char) (Address & 0x00FF);
	outData[1] = 0x06;
	outData[2] = (unsigned char)(((BASEREG485BRIDGE232+32) & 0xFF00)>>8);
	outData[3] = (unsigned char)((BASEREG485BRIDGE232+32)&0x00FF);
	outData[4] = gpibaddress;// GPIB address. The bridge will instruct this instrument to "listen"
		// then issue unlisten to all devices
	length = strlen((char*)cmd);
	for (j=0;j<length;j++){
			outData[j+5]=cmd[j];
	}
//	outData[length+5]=13; //append a CR  dont do this here.  any terminator must be at the end of cmdstring because
// different instruments require different terminators.. or a sequence of terminators.
	// calculate the CRC
	temp=modRTU_CRC(outData,length+5);
	// append the CRC in the correct order
	outData[length+6]=(temp & 0xFF00)>>8;
	outData[length+5]=(temp & 0x00FF);
	// establish the correct number of bytes to send out
	bytes_to_write=length+7;
//		printf("out:\t");printHexData(outData,bytes_to_write);
	write_rs485BYTE(outData,bytes_to_write,rtnData,&k);
//		printf("rtn:\t");printHexData(rtnData,k);
	status=0;
	if (rtnData[1] & 0x80){
		status = (rtnData[2]<<8) | rtnData[3];
		printf("sendGPIBData %s return error code %d\n",cmd,status);
//debuging information
		printHexData(rtnData,k);
	}
return status;
}

int listenGPIBData(unsigned char *returnData, char terminator, char gpibaddress, unsigned short Address){

	 int i,length, status;
	unsigned char outData[64];
	unsigned char rtnData[64];
	unsigned int k;
	unsigned short temp,bytes_to_write;

	outData[0] = (unsigned char) (Address & 0x00FF);
	outData[1] = 0x03;
	outData[2] = (unsigned char)(((BASEREG485BRIDGE232+32) & 0xFF00)>>8);
	outData[3] = (unsigned char)((BASEREG485BRIDGE232+32) & 0x00FF);

	outData[4]=gpibaddress;// GPIB address.  The bridge will instruct this instrument to "talk".
			// then issue untalk to all devices afterwards.
	outData[5]=terminator;// the terminator in the data string to know when to stop receiving. if terminator=0;
			// then the bridge senses the EOI control line of the GPIB buss
	// calculate the CRC
	length=0;
	temp=modRTU_CRC(outData,length+6);
	// append the CRC in the correct order
	outData[length+7]=(temp & 0xFF00)>>8;
	outData[length+6]=(temp & 0x00FF);
	// establish the correct number of bytes to send out
	bytes_to_write=length+8;
		//printf("out:\t");printHexData(outData,bytes_to_write);
	write_rs485BYTE(outData,bytes_to_write,rtnData,&k);
		//printf("rtn:\t");printHexData(rtnData,k);

	status=0;
	if(k>0){
	if (!(rtnData[1] & 0x80)){
		if ((rtnData[2]+5)==k) {
			for (i=0;i<rtnData[2];i++){
				returnData[i]=rtnData[i+3];
			}
			returnData[i]=0;
			} else {
				status = 1;
				printf("writeRS232Bridge process returned an unexpected number of bytes\n");
				printHexData(rtnData,k);
				printf("%s\n",rtnData);
				returnData[0]=0;
				}
		} else {
		status= rtnData[2]<<8 | rtnData[3];
		returnData[0]=0;
		printf("ListenGPIBBridge process returned error code %04x \n",temp);
		printHexData(rtnData,k);

		}
	} else {
	status=1;
	returnData[0]=0;
	printf("No Response from bridge at address %02X=%d \n",outData[0],outData[0]);
	}
		//printf("%s\n",returnData);
return status;


}

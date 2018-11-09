/*

General header File to encapsulate all slave device functions
 */


#define BASEREGANLG 0x0D0D
#define BASEREGSERVO 0x0A0A
#define BASEREG485BRIDGE232 0x0C0C
#define BASEREGSTEPMTR 0x0B0B
#define BASEREGFN 0x00F0

/* **************************************************************************************************

Universal ID function.  All slave devices will respond to this.

*/
int getRS485SlaveID(unsigned char* returnData, unsigned  short Address);
int changeRS485Address(unsigned short Address, unsigned short newAddress);

/* **************************************************************************************************

Stepper motor slave module:

*/
int findHomeRS485StepperMotor(unsigned short chan, unsigned char state, unsigned char direction);
//macro that has device find home on its own
int moveRS485StepperMotor(unsigned short chan, unsigned short steps, unsigned char dir);
//move by number of steps
int getRS485StepperMotorSteps(unsigned short chan, unsigned int* steps);
// use this to query motion.  The return value is steps yet to move. This counts down to zero when the move is completed
int getRS485StepperMotorStepsRev(unsigned short chan, unsigned int* stepsperrev);
int setRS485StepperMotorStepsRev(unsigned short chan, unsigned short stepsperrev);
// stepsperrev is only used in homing. If home not found in stepsperrev, motion ends
int getRS485StepperMotorSpeed(unsigned short chan, unsigned int* speed);
int setRS485StepperMotorSpeed(unsigned short chan, unsigned short speed);
// speed that pulses are sent out

/* **************************************************************************************************

Dual servo slave module:

*/
int setRS485ServoPosition(unsigned short chan, unsigned short servo, unsigned short position);
int getRS485ServoPosition(unsigned short chan, unsigned short servo, unsigned int* position);
/* **************************************************************************************************

Digital out module

*/

int setRS485DigitalOut(unsigned short chan, unsigned short value);
int getRS485DigitalOut(unsigned short chan, unsigned short * value);



/* **************************************************************************************************

Analog recorder slave module

*/
int setRS485AnalogRecorderPeriod(unsigned short RS485Chan, unsigned short ADCperiods);
int getRS485AnalogRecorderPeriod(unsigned short RS485Chan, unsigned int* ADCperiods);
// ADCperiods is in units of 16mS.  How often the PIC takes, and buffers, a reading
unsigned int readRS485AnalogRecorderSimple(unsigned short RS485Chan, unsigned short AnalogChan, float* ADCcount);
unsigned int readRS485AnalogRecorder(unsigned short RS485Chan, unsigned short AnalogChan, float vref, float* volts, float* stdev);
/* vref scales the ADCcount. For an analog signal, undivided by resistors into the PIC, vref=5.0.  This will need to be modified 
 if there is an input voltage divider.  If the returned value needs to not be converted to an actual voltage then set vref=1024.0. 

IMPORTANT: VERSION 3 BOARDS (WITH THE DISPLAYS) ONLY SAMPLE 7 MEASURMENTS.  OLD VERSION 2 BOARDS SAMPLED 10 MEASUREMENTS.
THE CODE IN THIS LIBRARY HAS BEEN UPDATED TO THE NEW BOARDS.

	R1		R2
[Vinput] ───/\/\/\──────┬──────/\/\/\───┐
			│		│
			│	      [gnd]
			│
			└──────────────────────────────[PIC analog input]. [0 to 5 V full scale = 0 to 1024 ADCcount]
*/



/* **************************************************************************************************

RS485 to RS232 Bridge  module

*/
int writeRS485to232Bridge(unsigned char* cmd, unsigned char* returnData, unsigned  short Address);
// passes ascii cmd to 232 device. returnData is response from device.
int setRS485BridgeReads(unsigned short reads, unsigned short Address);
int getRS485BridgeReads(unsigned short* reads, unsigned short Address);
int setRS485BridgeTimeout(unsigned short timeout,unsigned short Address);
int getRS485BridgeTimeout(unsigned short* timout,unsigned short Address);

// common to both bridges
int setRS485BridgeDebugPrint(unsigned short debug, unsigned short Address);
// debug = 0 or 1 to turn off/on local debug printing to attached screen

// RS485 to GPIB bridge functions

int getRS485GPIBStatus(unsigned short* GPIBStatus, unsigned short Address);
/*
This is more of a debugging thing. indicates the current status of the control lines on the GPIB bus.
probaably not useful for much of anything else.
Address is the RS485 device address
*/

int resetGPIBBridge(unsigned short Address); // Reasserts REN, and sends InterFaceClear (IFC) to all devices
//Address is the RS485 device address
//int sendGPIBCommand(unsigned char* cmd, unsigned short Address);  // lets not make it public
//but it is used by 'closeGPIBBridge(); 
// this is used for MANUALLY controlling the GPIB bus.  probably not needed for most instances.


int closeGPIBBridge(unsigned short Address);
// basically issues a GoToLocal (GTL) command .


int sendGPIBData(unsigned char *cmd, char gpibaddress, unsigned short Address);

/* sendGPIBCData( command to send, RS485 address of bridge device)
//Address is the RS485 device address
The Bridge tells the device at gpibaddress to listen, then sends cmd as data (not a command). The difference
is whether or not ATN is asserted. Then, an unlisten command is issued afterwards.  All of these steps are 
handled on the Bridge.

The return is an error status. status==0 means no errors.

The character array cmd must be a NULL terminated string, since the functions in this library determine the number of bytes
to send based on the length of the string. 
*/


int listenGPIBData(unsigned char *returnData, char terminator, char gpibaddress, unsigned short Address);
/* listenGPIBData( buffer for return data, RS485 address of bridge device)
//Address is the RS485 device address
// gpibaddress is the GPIB address of the instrument connected to RS485bridge

*/






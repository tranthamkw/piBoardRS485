/*


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "piBoard.h"
#include "RS485Devices.h"
#define BK1696ON 0
#define BK1696OFF 1

int initializeBK1696(unsigned short address);
int getVoltsAmpsBK1696(unsigned short address,float* volts, float* amps);
int setVoltsBK1696(unsigned short address, float volts);
int setAmpsBK1696(unsigned short address, float amps);
int setOutputBK1696(unsigned short address, int status);

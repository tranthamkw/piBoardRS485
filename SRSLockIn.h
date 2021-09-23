
/*


 */
#include "RS485Devices.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//assumes uses the RS232<->RS485 bridge
int getSRS830Data(float* r,float * phi, float * f, unsigned short RS485Address);
int initSRS830(unsigned short RS485Address);


int initSRS530(unsigned short RS485Address);
int getSRS530R(float* r, unsigned short RS485Address);
int getSRS530Phi(float* phi, unsigned short RS485Address);
int getSRS530Freq(float* f, unsigned short RS485Address);


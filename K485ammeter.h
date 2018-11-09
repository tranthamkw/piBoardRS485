/*


 */

#include "RS485Devices.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int getReadingK485(float* amps,char gpibaddress, unsigned short RS485Address);


int initializeK485(char gpibaddress, unsigned short RS485Address);

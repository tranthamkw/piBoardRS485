/*


 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piBoard.h"
#include "RS485Devices.h"


int getReadingF8840(float* volts, char gpibaddress, unsigned short RS485Address);

int initializeF8840(char gpibaddress, unsigned short RS485Address);


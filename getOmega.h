/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 
*/
#include "piBoard.h"
#include "RS485Devices.h"
#include "omegaCN7500.h"
#include "omegaCN740.h"

// THESE ARE THE RS485 ADDRESSES OF EACH RS485 INSTRUMENT/BRIDGE/SLAVE DEVICE
#define OMEGA2 8 //omegaCN7800

float omegaTemperature(int ch);

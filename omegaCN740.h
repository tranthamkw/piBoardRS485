/*
   notes and comments 
   useful information
   to follow

*/

#include <stdio.h>
#include <stdlib.h>
#include "piBoard.h"


int getPVCN740(unsigned short chan, float* temperature);
int getSVCN740(unsigned short chan, float* temperature);
int setSVCN740(unsigned short chan, float temperature);
// chan is the rs485 channel

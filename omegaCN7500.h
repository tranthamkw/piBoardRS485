/*
   notes and comments 
   useful information
   to follow

Omega Setup
SALA = off
CoSH = off
C-SL = rtu
C-no = 8  <--change this to a unique RS485 bus address
bPS = 9600
LEn=8
Prty = none
StoP=1



*/

#include <stdio.h>
#include <stdlib.h>
#include "piBoard.h"


int getPVCN7500(unsigned short chan, float* temperature);
int getSVCN7500(unsigned short chan, float* temperature);
int setSVCN7500(unsigned short chan, float temperature);
// chan is the rs485 channel

/*
machine layer. assumes that analog outputs from grandville phillips 
are connected to ADC channels 


*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kenBoard.h"

#define GP_HE_CHAN 0
#define GP_N2_CHAN 1

int getConvectron(unsigned int chan, float* returnvalue);
int getIonGauge(float* returnvalue);

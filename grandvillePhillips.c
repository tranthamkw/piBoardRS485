/*


*/


#include "grandvillePhillips.h"


int getConvectron(unsigned int chan, float* returnvalue){
	int kchan;
	float CVGauge;
	unsigned int rdata;

	switch (chan){
	case GP_HE_CHAN:  // this is the analog out port on the grandville phillips machine
		kchan = 0; //that is connected to this analog channel on the board
		break;
	case GP_N2_CHAN:
		kchan = 1; //Nitrogen CV pressure
		break;
	default:
		return -1;
	}
	getADC(kchan,&rdata);
	CVGauge=(float)rdata;
	CVGauge = pow(10,(0.00823*CVGauge - 4.0));
	*returnvalue=CVGauge;
return 0;
}

int getIonGauge(float* returnvalue){
	float IonGauge;
	unsigned int rdata;

	getADC(2,&rdata);
	IonGauge = 0.00878 * (float)rdata;
	IonGauge = pow(10,(IonGauge-11.4));

	*returnvalue=IonGauge;

return 0;
}

/*


*/
#include "getOmega.h"


float omegaTemperature(int ch){
	int i;
	float myTemp;
	i=getPVCN7500(OMEGA2,&myTemp);
	return myTemp;
}

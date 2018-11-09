/*
 * This is a template file to show all of the necessary components to add
 * to new data collection files that are created. 

starting point. 


*/

#include "piBoard.h"




int main (int argc, char* argv[]){



	if (argc==2) {
		startvalue=atoi(argv[1]);
		endvalue=atoi(argv[2]);
		stepsize=atoi(argv[3]);
		strcpy(comments,argv[4]);
	} else {
		printf("Usage:\n$ sudo ./<programName> <input1> ...\n");
		return 0;
	}



	initializeBoard();
//	initializeUSB1208();

	// Code goes here.

//	closeUSB1208();


	return 0 ;
}

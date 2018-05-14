#include <iostream>
#include "../include/init.h"
#include "../include/runs.h"
using namespace std;

int main(){

	// ~~~~~~ Essentials ~~~~~~~~
	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);
	
	// speficy log file for gsm
	ofstream gsml("../output/gsm_log.txt");
	gsm_log = &gsml;

//	// create a grid limits vector for convenience
//	float glimits[] = {0, 150, -60, 60};
//	vector <float> glim(glimits, glimits+4);
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~

	init_firenet();
	
	main_run();
	
	return 0;
}



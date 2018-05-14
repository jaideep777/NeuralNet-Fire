#include <iostream>
#include "../include/init.h"
#include "../include/runs.h"
using namespace std;

int main(int argc, char** argv){

	// append sim_name to output and params dirs (if specified) 
	if (argc > 1){
		sim_name = argv[1];
		out_dir += "_" + sim_name;
		params_dir += "_" + sim_name;
		cout << params_dir << endl;
	}

	// ~~~~~~ Essentials ~~~~~~~~
	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);
	
	// speficy log file for gsm
	ofstream gsml((out_dir+"/gsm_log.txt").c_str());
	gsm_log = &gsml;

//	// create a grid limits vector for convenience
//	float glimits[] = {0, 150, -60, 60};
//	vector <float> glim(glimits, glimits+4);
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~

	init_firenet();
	
	prerun_canbio_ic();
	prerun_lmois_ic();
	main_run();
	
	close_firenet();
	
	return 0;
}



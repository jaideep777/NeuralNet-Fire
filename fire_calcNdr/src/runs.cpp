#include "../include/runs.h"
#include "../include/ndr.h"
//#include "../include/moisture.h"
//#include "../include/fire.h"
using namespace std;



inline int printRunHeader(string s, int ns, int ds){
	cout << "\n****************************************************************\n\n";
	cout << s + " will run for " << ns << " steps.\n";
	cout << "progress will be displayed after every " << ds << " steps\n";
	if (ds == 1) cout << "progress (#steps) >> ";
	else cout << "progress (%) >> "; cout.flush();
}

extern map<string, ip_data> ip_data_map;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> main_run()
	
	Main Run!
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int main_run(){
	// simulation header
	printRunHeader("CALC NDR", nsteps, dstep);
	l_ncout = true;
	
	// mainloop
	for (int istep = 0; istep < nsteps; ++istep){
		double gtnow = gday_t0 + istep*(dt/24.0);
		if (gtnow > gday_tf) {cout << "Error in sim step count! \n"; return 1;}
		
		calc_ndr_dayavg(gtnow);

		// Write desired output variables to nc files and to singlePointOut
		ndr.writeVar(istep);

		if (istep % dstep == 0) {cout << "."; cout.flush();}
	}	

	cout << " > 100%\n";
	cout << "\n****************************************************************\n";

}





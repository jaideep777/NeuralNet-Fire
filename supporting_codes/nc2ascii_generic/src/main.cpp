#include <iostream>
#include "../include/init.h"
#include "../include/globals.h"
//#include "../include/runs.h"
using namespace std;


inline int printRunHeader(string s, double gt0, double gtf, int ns, int ds){
	cout << "\n****************************************************************\n\n";
	cout << s << " will run for " << ns << " steps.\n";
	cout << "\t" << gt2string(gt0) << " --- " << gt2string(gtf) << "\n";
	cout << "progress will be displayed after every " << ds << " steps\n";
	if (ds == 1) cout << "progress (#steps) >> ";
	else cout << "progress (%) >> "; cout.flush();
}

extern map<string, ip_data> ip_data_map;


int main_run(){

	int dstep = nsteps/40+1;

	printRunHeader("Main run", gday_t0, gday_t0 + (nsteps-1)*(dt/24.0), nsteps, dstep);

	for (int istep = 0; istep < nsteps; ++istep){
		double d = gday_t0 + istep*(dt/24.0);

		int yr  = gt2year(d);
		int mon = gt2month(d);
		int day = gt2day(d);

//		cout << gt2string(ymd2gday(yr,mon,day)) << endl;

		double tstart = 0;
		double tend = 0;
		if (time_step == "daily"){
			tstart = ymd2gday(yr,mon,day);
			tend   = ymd2gday(yr,mon,day) + 23.9/24;
		}
		else if (time_step == "fortnightly"){
			int day_start, day_end;
			if (day >= 1 && day <= 15){
				day_start = 1;
				day_end   = 15;
			}
			else{
				day_start = 16;
				day_end   = daysInMonth(yr,mon);
			}
			tstart = ymd2gday(yr,mon,day_start);
			tend   = ymd2gday(yr,mon,day_end) + 23.9/24;
		}
		else if (time_step == "monthly"){
			tstart = ymd2gday(yr,mon,1);
			tend   = ymd2gday(yr,mon,daysInMonth(yr,mon)) + 23.9/24;
		}
		else if (time_step == "yearly"){
			tstart = ymd2gday(yr,1,1);
			tend   = ymd2gday(yr,12,31) + 23.9/24;
		}
		else{
			tstart = d;
			tend = d + dt/24.0 - 0.01/24.f;
		}
				
		for (int i=0; i<vars.size(); ++i){
//			vars[i].readVar_reduce_mean(tstart, tend);
			vars[i].readVar_gt(tstart + hms2xhrs("6:0:0"), 0);
		}
		
		write_ascii_output(d);
		
		if (istep % dstep == 0) {cout << "."; cout.flush();}
	}	

	cout << " > 100%\n";
}




int main(){

	// ~~~~~~ Essentials ~~~~~~~~
	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);
	
	// speficy log file for gsm
	ofstream gsml("output/gsm_log.txt");
	gsm_log = &gsml;

//	// create a grid limits vector for convenience
//	float glimits[] = {0, 150, -60, 60};
//	vector <float> glim(glimits, glimits+4);
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~

	init_firenet();
	
//	prerun_canbio_ic();
//	prerun_lmois_ic();
	main_run();
	
	close_firenet();
	
	return 0;
}



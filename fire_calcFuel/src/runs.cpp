#include "../include/runs.h"
#include "../include/pheno.h"
#include "../include/moisture.h"
//#include "../include/fire.h"
using namespace std;


char ps2char(int c){
	     if (c == psX) return 'X';
	else if (c == psF) return 'F';
	else if (c == psM) return 'M';
	else if (c == psS) return 'S';
	else if (c == psZ) return 'Z';
	else if (c == psE) return 'E';
	else return 'X';
}


inline int printRunHeader(string s, double gt0, double gtf, int ns, int ds){
	cout << "\n****************************************************************\n\n";
	cout << s << " will run for " << ns << " steps.\n";
	cout << "\t" << gt2string(gt0) << " --- " << gt2string(gtf) << "\n";
	cout << "progress will be displayed after every " << ds << " steps\n";
	if (ds == 1) cout << "progress (#steps) >> ";
	else cout << "progress (%) >> "; cout.flush();
}

extern map<string, ip_data> ip_data_map;

//float dt_spin = 24;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> prerun_canbio_ic()
	
	Pre-run to set C0, the initial canopy and litter biomass. On yearly basis, 
		dL/dt = Mshed - kL
		dC/dt = Mnpp  - Mshed
	over the years, the canopy is in equilibrium, so
		Mshed = Mnpp, L = Mshed/k
	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int prerun_canbio_ic(){

	int nyrs = 5;	// number of years of bio pre-run
	double Bgt0 = int(spin_bio_gday_t0); // start n yrs before lmois spinup. Convert to int so that hrs is set to 0:0:0
	double Bgtf = spin_gday_t0;
	
	int nsteps_npp = (Bgtf-Bgt0)/dt_spinbio*24;
	int dstep_npp = nsteps_npp/40+1;

	printRunHeader("Canopy Biomass pre-run", Bgt0, Bgt0+(nsteps_npp-1)/dt_spinbio*24, nsteps_npp, dstep_npp);

	for (int istep = 0; istep < nsteps_npp; ++istep){

		double d = Bgt0 + istep*dt_spinbio/24;	
		
		// read NPP values
		npp.readVar_gt(d+14, 0);	// get total (monthly) NPP. Read in mode 0 (hold). Adding 15 is a dirty trick to deal with month-centered data

		// calculate canbio and littbio
		calc_pheno(d, dt_spinbio); 	// step size (delT) is 1 month
//		calc_ndr(d);

		for (int i=0; i<canbio.values.size(); ++i){
			canbio_max[i] = fmax(canbio_max[i], canbio[i]);
		}
		canbio_max.t = d;

		// Write desired output variables to nc files and to singlePointOut
		write_single_point_output(d, i_xlon, i_xlat);
		
		if (istep % dstep_npp == 0) {cout << "."; cout.flush();}
	}	

//	write_state(canbio_max);
//	canbio_prerun_on = false;	// finished prerun, so set flag to false
	
	cout << " > 100%\n";
	
}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> prerun_lmois_ic()
	
	Pre-run to stabilize litter moisture. It should start in April 
	with 0 litter moisture.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int prerun_lmois_ic(){

	int dstep_spin = nsteps_spin/40+1;

	printRunHeader("Litter Moisture pre-run", spin_gday_t0, spin_gday_t0 + (nsteps_spin-1)*(dt/24.0), nsteps_spin, dstep_spin);

	for (int istep = 0; istep < nsteps_spin; ++istep){
		double d = spin_gday_t0 + istep*(dt/24.0);
		if (d >= gday_t0) {cout << "Error in spin step count! \n"; return 1;}

		// read NPP values
		npp.readVar_gt(d+14, 0);	// get total (monthly) NPP. Read in mode 0 (hold) 

		// calculate canbio and littbio
		calc_pheno(d, dt); 	
		calc_ndr(d);
		calc_moisture();		

		write_single_point_output(d, i_xlon, i_xlat);

		if (istep % dstep_spin == 0) {cout << "."; cout.flush();}
	}	

	cout << " > 100%\n";
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> main_run()
	
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int main_run(){

	int dstep = nsteps/40+1;

	printRunHeader("Main run", gday_t0, gday_t0 + (nsteps-1)*(dt/24.0), nsteps, dstep);

	for (int istep = 0; istep < nsteps; ++istep){
		double d = gday_t0 + istep*(dt/24.0);

		// read NPP values
		npp.readVar_gt(d+14, 0);	// get total (monthly) NPP. Read in mode 0 (hold) 

		// calculate canbio and littbio
		calc_pheno(d, dt); 	
		calc_ndr(d);
		calc_moisture();		

		write_single_point_output(d, i_xlon, i_xlat);
		write_nc_output(istep);
		
		if (istep % dstep == 0) {cout << "."; cout.flush();}
	}	

	cout << " > 100%\n";
}





///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--> main_run()
//	
//	Main Run!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//int main_run(){
//	// simulation header
//	printRunHeader("MAIN RUN", nsteps, dstep);
//	l_ncout = true;
//	
//	// mainloop
//	for (int istep = 0; istep < nsteps; ++istep){
//		double gtnow = gday_t0 + istep*(dt/24.0);
//		if (gtnow > gday_tf) {cout << "Error in sim step count! \n"; return 1;}
//		
//		// update input files (if time has gone beyond the limits of any of the current files)
//		update_ip_files(gtnow);
//				
//		// read input (forcing) data from NC files
//		int k = read_all_ip_vars(gtnow, 0);

//		// run model components
//		calc_pheno(gtnow, dt); 	// calc dxL, and canopy biomass
//		calc_ndr(gtnow);
//		calc_moisture(); 		// calc lmois
//		calc_fire_train_data(gtnow); 		// calc fire index

//		// Write desired output variables to nc files and to singlePointOut
//		write_all_outvars(istep, gtnow);	

//		if (istep % dstep == 0) {cout << "."; cout.flush();}
//	}	

//	cout << " > 100%\n";
//	cout << "\n****************************************************************\n";

//}




///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--> fire_run()
//	
//	Main Run!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//int predict_run(){
//	// simulation header
//	printRunHeader("PREDICTION RUN", nsteps, dstep);
//	l_ncout = true;
//	
//	// mainloop
//	for (int istep = 0; istep < nsteps; ++istep){
//		double gtnow = gday_t0 + istep*(dt/24.0);
//		if (gtnow > gday_tf) {cout << "Error in sim step count! \n"; return 1;}
//		
//		// update input files (if time has gone beyond the limits of any of the current files)
//		update_ip_files(gtnow);
//				
//		// read input (forcing) data from NC files
//		int k = read_all_ip_vars(gtnow, 0);

//		// run model components
////		calc_fire(gtnow); 		// calc fire index

//		// Write desired output variables to nc files and to singlePointOut
//		write_all_outvars(istep, gtnow);	

//		if (istep % dstep == 0) {cout << "."; cout.flush();}
//	}	

//	cout << " > 100%\n";
//	cout << "\n****************************************************************\n";

//}




#include "../include/init.h"
#include "../include/globals.h"
//#include "../include/vars.h"
using namespace std;

// global variables for use in this file only
const string attrbegin = ">";
static int l_ip_init_done = false;
map <string, string> data_dirs;		// list of named dirs
map <string, ip_data> ip_data_map;		// ---
map <string, bool> writeVar_flags;		// 
map <string, bool> writeVarSP_flags;	// maps from var_name to other things
map <string, string> static_var_files; 	// 
vector <gVar*> model_variables;			// using a vector here allows control over order of variables

static string params_dir = "../params_newdata";
static string params_ft_file = params_dir + "/" + "params_ft.r";
static string params_ip_file = params_dir + "/" + "params_ip.r";
static string sim_config_file = params_dir + "/" + "sim_config.r";



// Class ip_data
ip_data::ip_data(){}

ip_data::ip_data(string _n, string _u, string _fnp, int _sy, int _ey, int _ny ) : 
				name(_n), unit(_u), fname_prefix(_fnp), start_yr(_sy), end_yr(_ey), nyrs_file(_ny) {
}
		

int ip_data::generate_filenames(string dir){
	if (dir != "") dir = dir + "/";
	if (nyrs_file > 1){ // data in a single file
		fnames.push_back(dir+fname_prefix+"."+int2str(start_yr)+"-"+int2str(start_yr+nyrs_file-1)+".nc");
	}
	else{
		for (int i=start_yr; i<=end_yr; ++i){
			fnames.push_back(dir+fname_prefix+"."+int2str(i)+".nc");
		}
	}
	return 0;
}

void ip_data::print(ostream &fout1){
		fout1 << "Input: ";
		fout1 << name << " " << unit << " " << fname_prefix << " " << start_yr << " " << nyrs_file << "\n";
		fout1 << "~~~~\n";
		for (int i=0; i<fnames.size(); ++i){
			fout1 << fnames[i] << "\n";
		}
		fout1 << endl;
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> read_ip_params_file()

	READ INPUT PARAMS FILE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int read_ip_params_file(){
	ifstream fin;
	fin.open(params_ip_file.c_str());
	
	string s, u, v, w, y;
	int n, m, l;
	float f;
	
	while (fin >> s && s != attrbegin);	// read until 1st > is reached
	
	fin >> s; 
	if (s != "FORCING_DATA_DIRS") {cout << "ip data dirs not found!"; return 1;}
	while (fin >> s && s != attrbegin){
		if (s == "") continue;
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
		fin >> u;
		
		data_dirs[s] = u;
		logdebug << s << ": " << data_dirs[s] << ".\n";	
	}
	
	string parent_dir = data_dirs["forcing_data_dir"];	

	fin >> s; 
	if (s != "FORCING_VARIABLE_DATA") {cout << "variable data not found!"; return 1;}
	while (fin >> s && s != attrbegin){
		if (s == "") continue;	// skip empty lines
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip # following stuff (comments)
		fin >> u >> v >> m >> l >> n;

		ip_data a(s, u, v, m, l, n);
		a.generate_filenames(parent_dir+"/"+data_dirs[s]);
		ip_data_map.insert( pair <string, ip_data> (s,a) );
		ip_data_map[s].print(log_fout);
	}	
		
	fin >> s; 
	if (s != "STATIC_INPUT_FILES") {cout << "static input files not found!"; return 1;}
	while (fin >> s && s != attrbegin){
		if (s == "") continue;	// skip empty lines
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
		fin >> u;
		static_var_files[s] = parent_dir + "/" + u;
		writeVar_flags[s] = false;		// static variables are not written to output, by default
		writeVarSP_flags[s] = false;	// 
	}

	
	fin.close();
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> read_sim_config_file()

	READ SIMULATION CONFIGURATION PARAMETERS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int read_sim_config_file(){
	ifstream fin;
	fin.open(sim_config_file.c_str());

	string s, u, v, w;
	int n, m, l;
	float f;
	while (fin >> s && s != attrbegin);	// read until 1st > is reached
	
	fin >> s; 
	if (s != "TIME") {cout << "sim time not found!"; return 1;}
	while (fin >> s && s != attrbegin){
		if (s == "") continue;	// skip empty lines
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
		fin >> u;
		if		(s == "sim_date0")	sim_date0 = u;
		else if (s == "sim_t0")		sim_t0 = u;
		else if (s == "sim_datef")	sim_datef = u;
		else if (s == "sim_tf")		sim_tf = u;
		else if (s == "dt")			dt = str2float(u);
		else if (s == "base_date")	gday_tb = ymd2gday(u);
		else if (s == "spinup")		lspinup = (u == "on")? true:false;
		else if (s == "spin_date0")	spin_gday_t0 = ymd2gday(u);	// assume time = 0:0:0
	}

	fin >> s; 
	if (s != "MODEL_GRID") {cout << "model grid not found!"; return 1;}
	while (fin >> s && s != attrbegin){
		if (s == "") continue;	// skip empty lines
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
		fin >> u;
		if		(s == "lon0")	mglon0 = str2float(u);
		else if (s == "lonf")	mglonf = str2float(u);
		else if (s == "lat0")	mglat0 = str2float(u);
		else if (s == "latf")	mglatf = str2float(u);
		else if (s == "dlat")	mgdlat = str2float(u);
		else if (s == "dlon")	mgdlon = str2float(u);
		else if (s == "xlon")	  xlon = str2float(u);
		else if (s == "xlat")	  xlat = str2float(u);
		else if (s == "pointOutFile") pointOutFile = u;
		else if (s == "SPout_on") spout_on = (u == "1")? true:false;
	}
	
	fin >> s; 
	if (s != "OUTPUT_VARIABLES") {cout << "output variables not found!"; return 1;}
	while (fin >> s && s != attrbegin){
		if (s == "") continue;	// skip empty lines
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
		fin >> n >> m;

		writeVar_flags[s] = n;
		writeVarSP_flags[s] = m;
		loginfo << "Write flag for " << s << ": " << writeVar_flags[s] << ", " << writeVarSP_flags[s] << '\n';
	}

//	fin >> s; 
//	if (s != "VARS_TO_USE") {cout << "vars to use (debugging) not found!"; return 1;}
//	while (fin >> s && s != attrbegin){
//		if (s == "") continue;	// skip empty lines
//		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
//		fin >> n;

//		if (n==0) writeVar_flags[s] = 0;	// if variable is not used at all, it cant be output!
//		if (n==0) writeVarSP_flags[s] = 0;	// if variable is not used at all, it cant be output!
//	}


}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> read_veg_params_file()
	
	READ VEG TYPE PARAMS FROM FILE
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
inline int char2phenostage(char c){
	     if (c == 'X') return psX;
	else if (c == 'F') return psF;
	else if (c == 'M') return psM;
	else if (c == 'S') return psS;
	else if (c == 'Z') return psZ;
	else if (c == 'E') return psE;
	else return psX;
}

int read_veg_params_file(){
	ifstream fin;
	fin.open(params_ft_file.c_str());

	string s, u, v, w;
	int n, m, l;
	float f;
	while (fin >> s && s != attrbegin);	// read until 1st > is reached
	
	fin >> s; 
	if (s != "nPFT") {cout << "number of PFTs not found!"; return 1;}
	fin >> npft;	// read number of pfts into global variable
	
	// resize all veg related vectors to npft values
	aLf.resize(npft); aSf.resize(npft); aRf.resize(npft);	// allocation fractions during flushing
	aL.resize(npft); aS.resize(npft); aR.resize(npft);		// allocation fractions during current phenology state
	LAImax.resize(npft); LAImin.resize(npft);				// min and max LAI for each pft
	phenoStages.resize(npft*12);	// phenology stages
	rFixC.resize(npft*12);			// monthly carbon fixation rates
	aFixC.resize(npft*12);			// monthly carbon fixation fractions 
	leafLs.resize(npft); Tdecomp.resize(npft);
	z1Month.resize(npft, -1);		// 1st leafless month, -1 if not found.
	Wc_sat_vec.resize(npft, 0);
	
	// start reading values into these vectors
	while (fin >> s && s != attrbegin){
		if (s == "") continue;	// skip empty lines
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments) 
		if		(s == "LM")	for (int i=0; i<npft; ++i) fin >> LAImax[i];
		else if (s == "Lm")	for (int i=0; i<npft; ++i) fin >> LAImin[i];
		else if (s == "aL")	for (int i=0; i<npft; ++i) fin >> aLf[i];
		else if (s == "aS")	for (int i=0; i<npft; ++i) fin >> aSf[i];
		else if (s == "LL") for (int i=0; i<npft; ++i) fin >> leafLs[i];
		else if (s == "T")  for (int i=0; i<npft; ++i) fin >> Tdecomp[i];
		else if (s == "ZM") for (int i=0; i<npft; ++i) fin >> z1Month[i]; 
		else if (s == "Wcs") for (int i=0; i<npft; ++i) fin >> Wc_sat_vec[i]; 

		else if (s == "rhobL") 		fin >> rhobL; 
		else if (s == "theta_sL") 	fin >> theta_sL; 
	}

	fin >> s; //cout << "s = " << s << '\n';
	char c; 
	if (s != "PHENO") {cout << "phenology not found!"; return 1;}
	for (int m=0; m<12; ++m){
		fin >> c;	// ignore the first char which is for month
		for (int i=0; i<npft; ++i){
			fin >> c;
			phenoStages[IX2(i,m, npft)] = char2phenostage(c); 
		}
	}
	
	while (fin >> s && s != attrbegin);	// loop till next >
	fin >> s; 
	if (s != "CARBON_FIXATION_RATE") {cout << "C Fixation rates not found!"; return 1;}
	for (int m=0; m<12; ++m){
		fin >> c;	// ignore the first char which is for month
		for (int i=0; i<npft; ++i){
			fin >> rFixC[IX2(i,m, npft)]; 
		}
	}

	// create aFixC vector (same as rFixC but negative values set to Zero)
	for (int m=0; m<12; ++m){
		for (int i=0; i< npft; ++i) {
			aFixC[IX2(i,m, npft)] = (rFixC[IX2(i,m, npft)] > 0)? rFixC[IX2(i,m, npft)] : 0;  
		}
	}
	
	// check if everything is correct
	log_fout << "--------- C fixation rates ---------------\n";
	for (int i=0; i<12; ++i){
		for (int j=0;j<npft;++j) { log_fout << rFixC[npft*i+j] << "\t";}
		log_fout << "\n";
	}
	log_fout << "\n";
	log_fout << "--------- 1st leafless month ---------------\n";
	for (int j=0;j<npft;++j) { log_fout << z1Month[j] << "\t";}
	log_fout << "\n";
	log_fout << "------------------------\n";

}




/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> init_modelvar(...)
	
	create a single gVar to be used in model and set metadata with model grid.
	if it is to be written to output, create an NcOutputStream.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int init_modelvar(gVar &v, string var_name, string unit, int nl, vector<double> times_vec, ostream& lfout){
	// create levels vector
	vector <float> levs_vec(nl,0); 
	for (int i=0; i<nl; ++i) levs_vec[i] = i; 

	// create model variable 
	v = gVar(var_name, unit, tunits_out);
	v.setCoords(times_vec, levs_vec, mglats, mglons);
	v.printGrid(lfout);
	v.fill(0);

	// set bool values for variables to output
	v.lwrite = writeVar_flags[var_name];	
	v.lwriteSP = writeVarSP_flags[var_name] & spout_on;
	
	// add gVar to model variables list
	model_variables.push_back(&v);

}



int create_sim_config(){

	// Set Sim Date and Time
	gday_t0 = ymd2gday(sim_date0) + hms2xhrs(sim_t0);
	gday_tf = ymd2gday(sim_datef) + hms2xhrs(sim_tf);	
	tunits_out = "hours since " + gt2string(gday_tb);
	
	// Create the model grid
	loginfo << "\nCoordinates:\n";
	mglons = createCoord(mglon0, mglonf, mgdlon, mgnlons);
	mglats = createCoord(mglat0, mglatf, mgdlat, mgnlats);
	mglevs.resize(1,1); mgnlevs = 1;
	printArray(mglons, log_fout, "lons: ");
	printArray(mglats, log_fout, "lats: ");
	grid_limits.resize(4);
	grid_limits[0] = mglon0;
	grid_limits[1] = mglonf;
	grid_limits[2] = mglat0;
	grid_limits[3] = mglatf;

	// create time vector 
	nsteps = (gday_tf - gday_t0)*24/dt + 1;
	nsteps_spin = (gday_t0 - spin_gday_t0)*24/dt;	// no +1 because this is 1 step less than t0
	mgtimes.resize(nsteps);
	for (int i=0; i<nsteps; ++i) mgtimes[i] = (gday_t0 + i*dt/24.0 - gday_tb)*24.0;
	
	// number of steps after which to show a dot so that 40 dots make up 100%
	dstep = nsteps/40;	
	if (dstep == 0) ++dstep;
	
	// get indices of cell containing SP-output point
	i_xlon = indexC(mglons, xlon);
	i_xlat = indexC(mglats, xlat);

}



///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--> init_vars()
//	
//	Call the single variable function one by one on each variable.
//  Init oneshot input and stream input and output based on relevant mappings
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int init_vars(){


	log_fout << "========== BEGIN VARIABLE INITIALIZATION ================\n";

//	init_modelvar( npp,  "npp",  ip_data_map["npp"].unit,  1, mgtimes, log_fout); 
//	init_modelvar( pr,   "pr",   ip_data_map["pr"].unit,   1, mgtimes, log_fout); 
//	init_modelvar( rh,   "rh",   ip_data_map["rh"].unit,   1, mgtimes, log_fout); 
//	init_modelvar( ts,   "ts",   ip_data_map["ts"].unit,   1, mgtimes, log_fout); 
//	init_modelvar( wsp,  "wsp",  ip_data_map["wsp"].unit,  1, mgtimes, log_fout); 
//	init_modelvar( ffev, "ffev", ip_data_map["ffev"].unit, 1, mgtimes, log_fout); 
//	
//	init_modelvar( canbio,      "canbio",     "gC/m2",  npft, mgtimes, log_fout); 
//	init_modelvar( canbio_max,  "canbio_max", "gC/m2",  npft, mgtimes, log_fout); 
//	init_modelvar( dxl,         "dxl",        "cm",     1,    mgtimes, log_fout); 
	init_modelvar( ndr,         "ndr",        "W/m2",   1,    mgtimes, log_fout); 
//	init_modelvar( evap,        "evap",       "mm/day", 1,    mgtimes, log_fout); 
//	init_modelvar( lmois,       "lmois",      "kg/m2",  1,    mgtimes, log_fout); 
//	init_modelvar( cmois,       "cmois",      "kg/m2",  1,    mgtimes, log_fout); 
//	init_modelvar( ps,          "ps",         "Pa",     1,    mgtimes, log_fout); 

//	init_modelvar( msk,     "msk",     "mm/day",  1,    mgtimes, log_fout); 
//	init_modelvar( elev,    "elev",    "m",       1,    mgtimes, log_fout); 
//	init_modelvar( albedo,  "albedo",  "mm/day",  1,    mgtimes, log_fout); 
//	init_modelvar( vegtype, "vegtype", "mm/day",  npft, mgtimes, log_fout); 


	// create input streams for variables in ip_data
	log_fout << "<< Variables to be read from NC files: ";
	for (int i=0; i<model_variables.size(); ++i){
		string vname = model_variables[i]->varname;
		if (ip_data_map.find(vname) != ip_data_map.end()){ 	// check if variable is in input_data_map
			log_fout << vname << ", ";

			// create input stream
			model_variables[i]->createNcInputStream(ip_data_map.find(vname)->second.fnames, grid_limits);
		}
		else{
		} 
	}
	log_fout << endl;


	// create output streams for desired variables
	log_fout << ">> Variables to be written to NC files: ";
	for (int i=0; i<model_variables.size(); ++i){
		string vname = model_variables[i]->varname;
		if (model_variables[i]->lwrite){	
			log_fout << vname << ", ";
			string fname = "../output/"+vname+"."+sim_date0+"-"+sim_datef+".nc";
			
			// create output stream
			model_variables[i]->createNcOutputStream(fname);
		}
	}
	log_fout << endl;



	// read static variables
	log_fout << "<< Reading static variables: ";
	for (int i=0; i<model_variables.size(); ++i){
		string vname = model_variables[i]->varname;
		if (static_var_files.find(vname) != static_var_files.end()){ 	// check if variable has a static file listed
			log_fout << vname << ", "; log_fout.flush();

			// oneshot read static variable
			model_variables[i]->readOneShot(static_var_files.find(vname)->second, grid_limits);
		}
		else{
		} 
	}
	log_fout << "\n\n" << endl;


	// write single point output header
//	if (spout_on){
//		log_fout << "> Opening file to write point values: " << pointOutFile << '\n';
//		point_fout.open(pointOutFile.c_str());
//	
//		point_fout << "lat:\t " << xlat << "\t lon:\t" << xlon << "\n";
//		point_fout << "vegtype fractions:\n X\t AGR\t NLE\t BLE\t MD\t DD\t GR\t SC\n";
//		for (int i=0; i<npft; ++i){
//			point_fout << vegtype.getCellValue(xlon,xlat, i) << "\t"; 
//		}
//		point_fout << "\n";

//		point_fout << "date\ttime" << "\t"; 

//		log_fout << ">> Variables to be written to point_output file: ";
//		for (int i=0; i<model_variables.size(); ++i){
//			string vname = model_variables[i]->varname;
//			int nl = model_variables[i]->nlevs;
//			if (model_variables[i]->lwriteSP){
//				log_fout << vname << ", ";
//				if (nl == 1) point_fout << vname << "\t";
//				else{
//					for (int z=0; z<nl; ++z) point_fout << vname << z << "\t";
//				}				
//			}
//		}
//		point_fout << endl;
//		log_fout << endl;
//	}

	log_fout << "\n========== END VARIABLE INITIALIZATION ================\n\n" << endl;

//	// convert elevation to surface pressure
//	log_fout << "Converting elevation to surface pressure...";
//	for (int ilat=0; ilat<mgnlats; ++ilat){
//		for (int ilon=0; ilon<mgnlons; ++ilon){
//	
//			ps( ilon, ilat, 0) = 101325 - 1200*elev(ilon,ilat,0)/100;

//		}
//	}
//	log_fout << "DONE.\n\n";
}


//int close_all_nc_files(){
//	PREPROC_CLOSE_MODELVAR(canbio);         
//	PREPROC_CLOSE_MODELVAR(canbio_max);         
//	PREPROC_CLOSE_MODELVAR(dxl);         
//	PREPROC_CLOSE_MODELVAR(lmois);         
//	PREPROC_CLOSE_MODELVAR(cmois);         
//	PREPROC_CLOSE_MODELVAR(fire);         
//	PREPROC_CLOSE_MODELVAR(ps);         
//	PREPROC_CLOSE_MODELVAR(ndr);         
//	PREPROC_CLOSE_MODELVAR(evap);         

//}


//} 


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> init_firenet()
	
	Call all init commands to do full sim init and show progress.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int init_firenet(){
	log_fout.open("../output/log.txt");	// open log stream
	log_fout << " ******************* THIS IS LOG FILE ****************************\n\n";

	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	cout << "~                  F I R E N E T                               ~\n";
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	cout << "\n> Reading config parameters... "; cout.flush();
	read_sim_config_file();
	create_sim_config();
	cout << "DONE.\n> Reading input filenames... "; cout.flush();
	read_ip_params_file();
	cout << "DONE.\n> Reading forest type params... "; cout.flush();
	read_veg_params_file();
	cout << "DONE.\n> Initialising variables... "; cout.flush();
	init_vars();	
	cout << "DONE.\n";
	
	
	// check for consistency in vegtype levels and PFTs
	if (npft != vegtype.nlevs) {
		cout << "** ERROR ** : number of PFTs dont match levels in vegtype!\n\n";
		return 1;
	}
	
	return 0;
}



// ******* IO ************

int write_single_point_output(double gt, int _ixlon, int _ixlat){

	point_fout << gt2string_date(gt) << "\t" << gt2string_time(gt) << "\t"; 

	for (int i=0; i<model_variables.size(); ++i){
		string vname = model_variables[i]->varname;
		int nl = model_variables[i]->nlevs;
		if (model_variables[i]->lwriteSP){
			for (int z=0; z<nl; ++z) point_fout << (*model_variables[i])(_ixlon, _ixlat, z) << "\t";
		}
	}
	point_fout << endl;
}






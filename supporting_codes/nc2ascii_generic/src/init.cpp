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
map <string, int> static_var_nlevs; 	// 
vector <gVar*> model_variables;			// using a vector here allows control over order of variables

static string params_dir = "params_newdata";
static string params_ft_file = params_dir + "/" + "params_ft.r";
static string params_ip_file = params_dir + "/" + "params_ip.r";
static string sim_config_file = params_dir + "/" + "sim_config.r";


// Class ip_data
ip_data::ip_data(){}

ip_data::ip_data(string _n, string _u, string _fnp, int _sy, int _ey, int _ny, int _nl) : 
				name(_n), 
				unit(_u), 
				fname_prefix(_fnp), 
				start_yr(_sy), 
				end_yr(_ey), 
				nyrs_file(_ny),
				nlevs(_nl) {
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
		fout1 << "levels = " << nlevs << "\n";
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
	int n, m, l, k, nz;
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
		fin >> u >> v >> m >> l >> n >> k;

		ip_data a(s, u, v, m, l, n, k);
		a.generate_filenames(parent_dir+"/"+data_dirs[s]);
		ip_data_map.insert( pair <string, ip_data> (s,a) );
		ip_data_map[s].print(log_fout);
	}	
		
	fin >> s; 
	if (s != "STATIC_INPUT_FILES") {cout << "static input files not found!"; return 1;}
	while (fin >> s && s != attrbegin){
		if (s == "") continue;	// skip empty lines
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
		fin >> n >> u;
		static_var_files[s] = parent_dir + "/" + u;
		static_var_nlevs[s] = n;
//		writeVar_flags[s] = false;		// static variables are not written to output, by default
//		writeVarSP_flags[s] = false;	// 
	}

	fin >> s; 
	if (s != "TIME") {cout << "Time not found!"; return 1;}
	while (fin >> s && s != attrbegin){
		if (s == "") continue;	// skip empty lines
		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
		fin >> u;
		if		(s == "start_date")	    sim_date0 = u;
		else if (s == "start_time")		sim_t0 = u;
		else if (s == "end_date")	    sim_datef = u;
		else if (s == "end_time")		sim_tf = u;
		else if (s == "dt")		        dt = str2int(u);
		else if (s == "base_date")		gday_tb = ymd2gday(u);
		else if (s == "timestep")		time_step = u;
	}

	fin >> s; 
	if (s != "MODEL_GRID") {cout << "Model grid not found!"; return 1;}
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
	}

//	fin >> s; 
//	if (s != "OUTPUT_FILE") {cout << "Output file not specified!"; return 1;}
//	while (fin >> s && s != attrbegin){
//		if (s == "") continue;	// skip empty lines
//		if (s == "#") {getline(fin,s,'\n'); continue;}	// skip #followed lines (comments)
//		fin >> u;
//		if		(s == "outfile")	mglon0 = (u);
//	}
		
	fin.close();
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
	v.fill(0);

	// set bool values for variables to output
	v.lwrite = writeVar_flags[var_name];	
	v.lwriteSP = writeVarSP_flags[var_name] & spout_on;
	
	v.setRegriddingMethod("bilinear");

	// add gVar to model variables list
	model_variables.push_back(&v);

}



int create_sim_config(){

	// Set Sim Date and Time
	gday_t0 = ymd2gday(sim_date0) + hms2xhrs(sim_t0);
	gday_tf = ymd2gday(sim_datef) + hms2xhrs(sim_tf);	
	tunits_out = "hours since " + gt2string(gday_tb);
	if (time_step == "daily"){
		dt = 24;
	}
	else if (time_step == "fortnightly"){
		dt = 365.2524*24/12/2;
	}
	else if (time_step == "monthly"){
		dt = 365.2524*24/12;
	}
	else if (time_step == "yearly"){
		dt = 365.2524*24;
	}
	
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
//	nsteps_spin = (gday_t0 - spin_gday_t0)*24/dt;	// no +1 because this is 1 step less than t0
	mgtimes.resize(nsteps);
	for (int i=0; i<nsteps; ++i) mgtimes[i] = (gday_t0 + i*dt/24.0 - gday_tb)*24.0;
	
	// number of steps after which to show a dot so that 40 dots make up 100%
	dstep = nsteps/40;	
	if (dstep == 0) ++dstep;
	

}



///*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--> init_vars()
//	
//	Call the single variable function one by one on each variable.
//  Init oneshot input and stream input and output based on relevant mappings
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int init_vars(){


	log_fout << "========== BEGIN VARIABLE INITIALIZATION ================\n";

//	#define INIT_IP_MODELVAR(x, nlevs)  init_modelvar(x,  #x,  ip_data_map[#x].unit,  nlevs, mgtimes, log_fout)

	vars.resize(ip_data_map.size());
	int varcount = 0;
	for (map <string, ip_data>::iterator it = ip_data_map.begin(); it != ip_data_map.end(); ++it){
		string var_name = it->first;
		cout << "var = " << var_name << endl;
		init_modelvar(vars[varcount],  var_name,  it->second.unit,  it->second.nlevs, mgtimes, log_fout);
		++varcount;
	}

 	static_vars.resize(static_var_files.size());
 	varcount = 0;
	vector <double> tsnap(1, (ymd2gday("2009-1-1")-gday_tb)*24);	// single time snapshot
	for (map <string, string>::iterator it = static_var_files.begin(); it != static_var_files.end(); ++it){
		string var_name = it->first;
		cout << "stat var = " << var_name << endl;
		init_modelvar(static_vars[varcount],  var_name,  "-",  static_var_nlevs[it->first], tsnap, log_fout);
		++varcount;
	}



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


	// Create header for output ascii file
	log_fout << "> Opening file to write point values: " << "ascii.txt" << '\n';
	point_fout.open("ascii.txt");

//	point_fout << "lat:\t " << xlat << "\t lon:\t" << xlon << "\n";
//	point_fout << "vegtype fractions:\n X\t AGR\t NLE\t BLE\t MD\t DD\t GR\t SC\n";
//	for (int i=0; i<npft; ++i){
//		point_fout << vegtype.getCellValue(xlon,xlat, i) << "\t"; 
//	}
//	point_fout << "\n";

	point_fout << "date\ttime\tlat\tlon" << "\t"; 

	log_fout << ">> Variables to be written to point_output file: ";
	for (int i=0; i<model_variables.size(); ++i){
		string vname = model_variables[i]->varname;
		int nl = model_variables[i]->nlevs;
		log_fout << vname << ", ";
		if (nl == 1) point_fout << vname << "\t";
		else{
			for (int z=0; z<nl; ++z) point_fout << vname << z << "\t";
		}				
	}
	point_fout << endl;
	log_fout << endl;

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
	
	// display all variables
	for (int i=0; i<model_variables.size(); ++i) model_variables[i]->printGrid(log_fout);
	
}





/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> init_firenet()
	
	Call all init commands to do full sim init and show progress.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int init_firenet(){
	log_fout.open("output/log.txt");	// open log stream
	log_fout << " ******************* THIS IS LOG FILE ****************************\n\n";

	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	cout << "~                  F I R E N E T                               ~\n";
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
//	cout << "\n> Reading config parameters... "; cout.flush();
//	read_sim_config_file();
	cout << "DONE.\n> Reading input filenames... "; cout.flush();
	read_ip_params_file();
	create_sim_config();
//	cout << "DONE.\n> Reading forest type params... "; cout.flush();
//	read_veg_params_file();
	cout << "DONE.\n> Initialising variables... "; cout.flush();
	init_vars();	
	cout << "DONE.\n";
	
	
//	// check for consistency in vegtype levels and PFTs
//	if (npft != vegtype.nlevs) {
//		cout << "** ERROR ** : number of PFTs dont match levels in vegtype!\n\n";
//		return 1;
//	}
	
	return 0;
}


int close_firenet(){

	// close input streams
	log_fout << "!! Closing input streams: ";
	for (int i=0; i<model_variables.size(); ++i){
		string vname = model_variables[i]->varname;
		if (ip_data_map.find(vname) != ip_data_map.end()){ 	// check if variable is in input_data_map
			log_fout << vname << ", ";

			// create input stream
			model_variables[i]->closeNcInputStream();
		}
		else{
		} 
	}
	log_fout << "- Done." << endl;


//	// close output streams 
//	log_fout << "!! Closing output streams: ";
//	for (int i=0; i<model_variables.size(); ++i){
//		string vname = model_variables[i]->varname;
//		if (model_variables[i]->lwrite){	
//			log_fout << vname << ", ";
//			
//			// close output stream
//			model_variables[i]->closeNcOutputStream();
//		}
//	}
//	log_fout << "- Done." << endl;
	
	// close log files
	log_fout.close();

}


// ******* IO ************

int write_ascii_output(double gt){


	for (int ilat=0; ilat < mgnlats; ++ilat){
	for (int ilon=0; ilon < mgnlons; ++ilon){

		point_fout << gt2string_date(gt) << "\t" << gt2string_time(gt) << "\t"; 
		point_fout << mglats[ilat] << "\t" << mglons[ilon] << "\t";
		for (int i=0; i<model_variables.size(); ++i){
			string vname = model_variables[i]->varname;
			int nl = model_variables[i]->nlevs;
			for (int z=0; z<nl; ++z) point_fout << (*model_variables[i])(ilon, ilat, z) << "\t";
		}
		point_fout << endl;

	}	
	}

}


int write_nc_output(int islice){
	for (int i=0; i<model_variables.size(); ++i){
		string vname = model_variables[i]->varname;
		if (model_variables[i]->lwrite){
			model_variables[i]->writeVar(islice);
		}
	}
}





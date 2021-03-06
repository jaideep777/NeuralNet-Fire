// declaration of global variables

#include "../include/globals.h"

// log file
ofstream log_fout;
bool info_on = true, debug_on = true;

// simulation time
string sim_date0, sim_t0, sim_datef, sim_tf;
float dt = 0;
int sim_start_yr;
double gday_t0, gday_tf, gday_tb;
string tunits_out;
bool lspinup;
double spin_gday_t0;

// Model grid params
float mglon0, mglonf, mglat0, mglatf, mgdlon, mgdlat, mgdlev;
int mgnlons, mgnlats, mgnlevs;
vector <float> mglons, mglats, mglevs;
vector <double> mgtimes;
vector <float> grid_limits;

int nsteps;	// number of steps for which sim will run
int nsteps_spin; // number of spinup steps
int dstep; // progress display step 

// single point output
float xlon, xlat;
int i_xlon, i_xlat;
string pointOutFile;
bool spout_on = true;
ofstream sp_fout, point_fout;
bool l_ncout = false;

// prerun_flags
bool canbio_prerun_on = true;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Vegetation parameters

// size of the vectors is = npft

int npft = -1;

vector <float> aLf, aSf, aRf;	// allocation fractions during flushing
vector <float> aL, aS, aR;		// allocation fractions during current phenology state
vector <float> LAImax, LAImin;	// min and max LAI for each pft

vector <int> phenoStages;		// phenology stages
vector <float> rFixC;			// monthly carbon fixation rates
vector <float> aFixC;			// monthly carbon fixation fractions (add to 1 over all PFTs)
vector <float> leafLs;				// fraction of mass that decomposes in 1 yr
vector <float> Tdecomp;			// decomposition halflife in months
vector <int> z1Month;		// 1st month in which given PFT is leafless	(all leaves to be shed till then)
vector <float> Wc_sat_vec; 	// saturation water content of canopy / leaf layer (kg/m2)


float rhobL, theta_sL;

// SP test variables
vector <float> canbio_cumm;
vector <float> stembio_cumm;
vector <float> littbio_cumm;


// georeferenced variables
gVar msk;		// mask
gVar vegtype;	// forest type fractions
gVar elev;		// elevation
gVar albedo;		// surface albedo

gVar pr;		// Precipitation (pr) 
gVar rh;		// Relative Humidity (rh) 
gVar ts;		// Surface Temperature (ts) 
gVar wsp;		// Wind Speed (wsp) 
gVar npp;		// NPP (npp) 
gVar ffev;
gVar cld;

gVar canbio;		// canopy biomass
gVar canbio_max;	// max canopy biomass for LAI calculation, set during canbio prerun
gVar lmois;		// litter moisture (kg/m2 = mm)
gVar cmois;		// canopy moisture content (kg/m2 = mm) 
gVar dxl;		// litter layer thickness
gVar fire;		// fire!
gVar dfire;		// daily fire indices
gVar ndr; 		// net downward radiation
gVar ps;		// surface pressure
gVar evap;		// potential evaporation rate


#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <gsm.h>
using namespace std;

const float hrsPerMonth = 24*365.2524f/12.0f;
const float pi = 3.14159265358;

// log file!
extern ofstream log_fout;
extern bool info_on, debug_on;
#define loginfo  if (info_on)  log_fout << "<info> "
#define logdebug if (debug_on) log_fout << "<debug> "

// simulation time
extern string sim_date0, sim_t0, sim_datef, sim_tf;
extern float dt;
extern int sim_start_yr;
extern double gday_t0, gday_tf, gday_tb;
extern string tunits_out;
extern double spin_gday_t0;

// Model grid params
extern float mglon0, mglonf, mglat0, mglatf, mgdlon, mgdlat, mgdlev;
extern int mgnlons, mgnlats, mgnlevs;
extern vector <float> mglons, mglats, mglevs;
extern vector <double> mgtimes;
extern vector <float> grid_limits;

extern int nsteps;	// number of steps for which sim will run
extern int nsteps_spin; // number of spinup steps
extern int dstep; // progress display step 

// single point output
extern float xlon, xlat;
extern int i_xlon, i_xlat;
extern string pointOutFile;
extern bool spout_on;
extern ofstream sp_fout, point_fout;
extern bool l_ncout;
 
// prerun_flags
extern bool canbio_prerun_on;
extern bool lspinup;

// veg params
extern int npft;

extern vector <float> aLf, aSf, aRf;	// allocation fractions during normal growth
extern vector <float> aL, aS, aR;		// allocation fractions during current phenology state
extern vector <float> LAImax, LAImin;	// min and max LAI for each pft

extern vector <int> phenoStages;		// phenology stages
extern vector <float> rFixC;			// monthly carbon fixation rates
extern vector <float> aFixC;			// NPP fixation fractions for each PFT. N_fixed(i,m) = aFixC(i,m) * N_tot_obs <- WRONG!
extern vector <float> leafLs;			// leaf life span
extern vector <float> Tdecomp;			// fraction of mass decomposed in 1 yr
extern vector <int> z1Month;		// 1st month in which given PFT is leafless	(all leaves to be shed till then)
extern vector <float> Wc_sat_vec; 	// saturation water content of canopy / leaf layer (kg/m2)

enum PhenoStage{psX, psF, psS, psM, psZ, psE};
// ^ F = flush, S = shed, M = mature, Z = leafless, X = none, B = simultaneous S,M,F

extern float rhobL, theta_sL;	// veg and soil paramters

// SP test variables
extern vector <float> canbio_cumm;
extern vector <float> stembio_cumm;
extern vector <float> littbio_cumm;

// georeferenced variables
extern gVar msk;		// mask
extern gVar vegtype;	// forest type fractions
extern gVar elev;		// elevation
extern gVar albedo;		// surface albedo

extern gVar pr;		// Precipitation (pr) 
extern gVar rh;		// Relative Humidity (rh) 
extern gVar ts;		// Surface Temperature (ts) 
extern gVar wsp;	// Wind Speed (wsp) 
extern gVar npp;	// NPP (npp) 
extern gVar ffev;
extern gVar cld;

extern gVar canbio;		// canopy biomass
extern gVar canbio_max;	// max canopy biomass for LAI calculation, set during canbio prerun
extern gVar lmois;		// litter moisture (kg/m2 = mm)
extern gVar cmois;		// canopy moisture content (kg/m2 = mm) 
extern gVar dxl;		// litter layer thickness
extern gVar fire;		// fire!
extern gVar dfire;		// daily fire indices
extern gVar ndr; 		// net downward radiation
extern gVar ps;			// surface pressure
extern gVar evap;		// potential evaporation rate


#endif


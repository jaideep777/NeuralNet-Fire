#include <iostream>
#include <math.h>
#include <netcdfcpp.h>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

#include "/home/jaideep/libgsm/include/gsm.h"

#define setbit(x,n) x = x | (0b000000001 << (n));
#define clearbit(x,n) x = x & ~(0b000000001 << (n));
#define togglebit(x,n) x = x ^ (0b000000001 << (n));

/*
forest types:
0 = no forest
1 = Evergreen NL
2 = Evergreen BL
3 = Moist Deciduous 
4 = Deciduous
5 = Dry Deciduous
6 = grass
7 = scrub
*/


const int x_pft_code    = 0;
const int agr_pft_code  = 1;
const int nle_pft_code 	= 2;
const int ble_pft_code 	= 3;
const int md_pft_code 	= 4;
const int dd_pft_code 	= 5;
const int gr_pft_code 	= 6;
const int sc_pft_code 	= 7;

template <class T> 
int max_index(T &max, vector <T> v){
	max = v[0]; int imax = 0;
	for (int i=1; i<v.size(); ++i){
		if (v[i] > max) {
			imax = i;
			max = v[i];
		}
	}
	return imax;
}

int read_gVar(gVar &veg, string infile, string vname = ""){
	NcFile_handle veg_handle;
	veg_handle.open(infile, "r", glimits_india);
	veg_handle.readCoords(veg);
	if (vname != "") veg.ivar1 = veg_handle.getVarID(vname);
	veg_handle.readVarAtts(veg);
	veg_handle.readVar(veg, 0);
}

int write_gVar(gVar &v, string v_file){
	NcFile_handle v_handle;
	v_handle.open(v_file, "w", glimits_india);
	v_handle.writeCoords(v);
	v_handle.writeTimeValues(v);
	NcVar * v_ncvar = v_handle.createVar(v);
	v_handle.writeVar(v, v_ncvar, 0); 
}

vector <int> vt(256,8);	// mixed forest is 8, pure types (0-7) will be set.
int init_vt();

int main(){

	const int npfts = 8;
	
	init_vt();

	float lon0 = 66.5, lonf =  100.5, lat0 = 6.5, latf = 38.5, dlon = 0.5, dlat = 0.5, dt0 = 1, dlev = 1;
	int nlons, nlats, nlevs = npfts;
	//const float glimits_custom[4] = {lon0, lonf, lat0, latf};

	vector <float> lons = createCoord(lon0, lonf, dlon, nlons);
	vector <float> lats = createCoord(lat0, latf, dlat, nlats);
	vector <float> levs = createCoord(0, 1, 1, nlevs);

	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);

//	cout << "Enter Threshold: ";
	float eps1 = 0.05;
//	cin >> eps1;

	string veg_file = "/media/WorkData/Fire/ftmap_iirs/ftmap_iirs_8pft.nc";
	string dft_file = "/media/WorkData/Fire/ftmap_iirs/dft_forest_only.nc";
	string msk_file = "/media/WorkData/Fire/mask/surta_india_0.2.nc";
	string ffev_file = "/media/WorkData/Fire/fire_modis_0.5/fire_events.2000-2010.nc";

	ofstream fout("ffev_summary.txt"); 

	gVar veg;
	read_gVar(veg, veg_file, "vegtype");
	veg = lterp(veg, lons, lats);

	gVar dft;
	read_gVar(dft, dft_file);
	dft = lterp(dft, lons, lats);

	gVar msk;
	read_gVar(msk, msk_file);
	msk = lterp(msk, lons, lats);

	veg = mask(veg, msk);
	dft = mask(dft, msk);

	msk.printGrid();
	dft.printGrid();

	gVar ffev;
	NcFile_handle ffev_handle;
	ffev_handle.open(ffev_file, "r", glimits_india);
	ffev_handle.readCoords(ffev);
	ffev_handle.readVarAtts(ffev);
	ffev.printGrid();

	gVar ff_yrly("ff_yrly", "", "days since 2000-01-01")

	int curr_yr = gt2year(ffev.ix2gt(0));
	cout << "Year = " << curr_yr << '\n';
	cout << 
	
	for (int t=0; t<ffev.ntimes; ++t){

		int yr_t = gt2year(ffev.ix2gt(t));
		if (yr_t > curr_yr){
			// output accumulated year data, clear variable 
			curr_yr = yr_t;
			cout << "Year = " << yr_t << '\n';
		}

		for (int ilat=0; ilat<nlats; ++ilat){
		for (int ilon=0; ilon<nlons; ++ilon){
			
		}
		}
	}
	
//	write_gVar(veg, ftc_file);
//	write_gVar(dft, "dft.nc");
//	write_gVar(nft, "nft.nc");
	
//	// create combinatorial forest map
//	ft9.values = imax;
//	//ft9.printGrid();
//	
//	gVar ftmasked = mask(ft9, msk);
//	
//	ofstream fout;
//	string outascii = "ftvals_"+int2str(eps1*100)+".txt";
//	fout.open(outascii.c_str());

//	int npoints = ftmasked.nlons*ftmasked.nlats;
//	for (int i=0; i< npoints; ++i){
//		if (ftmasked.values[i] != ftmasked.missing_value) fout << ftmasked.values[i] << '\n';
//	}

//	NcFile_handle ft_handle;
//	ft_handle.open(ft_file, "w", glimits_india);
//	ft_handle.writeCoords(ftmasked);
//	ft_handle.writeTimeValues(ftmasked);
//	NcVar * ft_ncvar = ft_handle.createVar(ftmasked);
//	ft_handle.writeVar(ftmasked, ft_ncvar, 0); 
//	
//	// create categorical forest type map (with unclassified areas)
//	gVar ft; ft.shallowCopy(ftmasked);
//	ft.values.resize(npoints, std_missing_value);
//	for (int i=0; i< npoints; ++i){
//		if (ftmasked.values[i] != ftmasked.missing_value)
//			ft.values[i] = vt[int(ftmasked.values[i])];
//	}


	cout << "> Successfully wrote veg NC file!!\n";

}

int init_vt(){
//	vt[0]   = unclassified_code;
	vt[1]   = x_pft_code;
	vt[2]   = agr_pft_code;
	vt[4]   = nle_pft_code;
	vt[8]   = ble_pft_code;
	vt[16]  = md_pft_code;
	vt[32]  = dd_pft_code;
	vt[64]  = gr_pft_code;
	vt[128] = sc_pft_code;	
}

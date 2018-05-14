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


vector <int> vt(256,8);	// mixed forest is 8, pure types (0-7) will be set.
int init_vt();

int main(){

	const int npfts = 8;
	
	init_vt();

	float lon0 = 66.5, lonf =  100.5, lat0 = 6.5, latf = 38.5, dx = 0.25, dy = 0.25, dt0 = 1, dlev = 1;
	int nlons, nlats, nlevs = npfts;
	//const float glimits_custom[4] = {lon0, lonf, lat0, latf};

	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);

	cout << "Enter Threshold: ";
	float eps1 = 0.05;
	cin >> eps1;

	string veg_file = "/home/jaideep/fire/codes/ftmap_iirs_8pft.nc";
	string ft_file = "/home/jaideep/fire/codes/ftbin_iirs_8pft_"+int2str(eps1*100)+".nc";
	string msk_file = "/home/jaideep/fire/infisim_v5.1_iirsFT/input/surta_india_0.2.nc";
	string ftc_file = "/home/jaideep/fire/codes/ftcat_iirs_8pft_"+int2str(eps1*100)+".nc";

	gVar veg;
	NcFile_handle veg_handle;
	veg_handle.open(veg_file, "r", glimits_india);
	veg_handle.readCoords(veg);
	veg.ivar1 = veg_handle.getVarID("vegtype");
	veg_handle.readVarAtts(veg);
	veg_handle.readVar(veg, 0);
	//veg.printGrid();

	gVar msk;
	NcFile_handle msk_handle;
	msk_handle.open(msk_file, "r", glimits_india);
	msk_handle.readCoords(msk);
	msk_handle.readVarAtts(msk);
	msk_handle.readVar(msk, 0);
	//msk.printGrid();

	veg = lterp(veg, msk);
	
//	nlons = veg.nlons; nlats = veg.nlats; nlevs = veg.nlevs;

	vector <float> a(1,0);

	float * eps = new float[nlevs];
	for (int i=0; i< nlevs; ++i) eps[i] = eps1;
	eps[0] = .95;
	eps[1] = .5;

	gVar ft9; ft9.shallowCopy(veg);
	ft9.nlevs = 1; ft9.levs = a;
	// start from lev = 0; if upper levs are greater, 
	// 0 will be replaced succesively by lev number with larger value
	vector <float> vmax(nlons*nlats, 0);
	vector <float> imax(nlons*nlats, -100);
	
	for (int ilat=0; ilat<veg.nlats; ++ilat){
	for (int ilon=0; ilon<veg.nlons; ++ilon){
		unsigned char x = 0;
		for (int ilev=0; ilev<veg.nlevs; ++ilev){
			if (veg.values[ID(ilon, ilat, ilev)] > eps[ilev]){
					setbit(x,ilev);
			}
		}
		imax[ID(ilon, ilat, 0)] = float(int(x));
	}
	}
	
	// create combinatorial forest map
	ft9.values = imax;
	//ft9.printGrid();
	
	gVar ftmasked = mask(ft9, msk);
	
	ofstream fout;
	string outascii = "ftvals_"+int2str(eps1*100)+".txt";
	fout.open(outascii.c_str());

	int npoints = ftmasked.nlons*ftmasked.nlats;
	for (int i=0; i< npoints; ++i){
		if (ftmasked.values[i] != ftmasked.missing_value) fout << ftmasked.values[i] << '\n';
	}

	NcFile_handle ft_handle;
	ft_handle.open(ft_file, "w", glimits_india);
	ft_handle.writeCoords(ftmasked);
	ft_handle.writeTimeValues(ftmasked);
	NcVar * ft_ncvar = ft_handle.createVar(ftmasked);
	ft_handle.writeVar(ftmasked, ft_ncvar, 0); 
	
	// create categorical forest type map (with unclassified areas)
	gVar ft; ft.shallowCopy(ftmasked);
	ft.values.resize(npoints, std_missing_value);
	for (int i=0; i< npoints; ++i){
		if (ftmasked.values[i] != ftmasked.missing_value)
			ft.values[i] = vt[int(ftmasked.values[i])];
	}

	NcFile_handle ftc_handle;
	ftc_handle.open(ftc_file, "w", glimits_india);
	ftc_handle.writeCoords(ft);
	ftc_handle.writeTimeValues(ft);
	NcVar * ftc_ncvar = ftc_handle.createVar(ft);
	ftc_handle.writeVar(ft, ftc_ncvar, 0); 

	cout << "> Successfully wrote veg NC file!!\n";

}

int init_vt(){
	vt[0]   = unclassified_code;
	vt[1]   = x_pft_code;
	vt[2]   = agr_pft_code;
	vt[4]   = nle_pft_code;
	vt[8]   = ble_pft_code;
	vt[16]  = md_pft_code;
	vt[32]  = dd_pft_code;
	vt[64]  = gr_pft_code;
	vt[128] = sc_pft_code;	
}

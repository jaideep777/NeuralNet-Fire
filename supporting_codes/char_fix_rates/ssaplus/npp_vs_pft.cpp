#include <iostream>
#include <math.h>
#include <netcdfcpp.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
using namespace std;

#include <gsm.h>

/* --------------------------------------------------
compile command:
// g++ -I/usr/local/netcdf-c-4.3.2/include -I/usr/local/netcdf-cxx-legacy/include -I/home/jaideep/codes/FIRE_CODES/libgsm_v2/include -L/home/jaideep/codes/FIRE_CODES/libgsm_v2/lib -L/usr/local/netcdf-cxx-legacy/lib -o 1 npp_vs_pft.cpp -l:libgsm.so.2 -lnetcdf_c++ 
-----------------------------------------------------*/


int str2int(string s){
	istringstream sin;
	sin.str(s);
	int val;
	sin >> val;
	return val;
}

string int2str(int i){
	ostringstream sout;
	sout << i;
	return sout.str();
}

int main(){

	// ~~~~~~ Some NetCDF Essentials ~~~~~~~~
	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);
	
	// specify log file for gsm
	ofstream gsml("gsm_log.txt");
	gsm_log = &gsml;

	// create a grid limits vector for convenience
	float glimits[] = {59, 101, 4, 51};
	vector <float> glim(glimits, glimits+4);
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~

	// create the coordinates for our georeferenced variable
	gVar ft;
	ft.createOneShot("/media/jaideep/Totoro/Data/forest_type/MODIS/ftmap_modis_SASplus_0.5deg_11levs_noMixed.nc", glim);
	ft.printGrid();
	
	vector <double> times(1,0);
	gVar npp("npp", "gC/m2/s", "days since 2000-1-1");
	npp.setCoords(times, ft.levs, ft.lats, ft.lons);
	npp.createNcInputStream(vector <string> (1, "/media/jaideep/Totoro/Data/GPP_modis/npp.cycle.2001-2003.nc"), glim);
	npp.printGrid();

	gVar msk;
	msk.createOneShot("/media/jaideep/WorkData/Fire_G/util_data/masks/surta_global_0.5.nc", glim);
	msk = lterp(msk, ft.lons, ft.lats);
	msk.printGrid();

	for (int imon=0; imon<12; ++imon){

		npp.readVar_it(imon);
		
		ofstream fout(string("mon_"+int2str(imon)+".txt").c_str());

		fout << "msk\tlon\tlat\tnpp\t";
		for (int i=0; i<ft.nlevs; ++i) fout << "ft" << i << "\t";
		fout << "\n";

		for (int ilon=0; ilon<ft.nlons; ++ilon){
			for (int ilat=0; ilat<ft.nlats; ++ilat){
				if (msk(ilon,ilat,0) > 0.5){
					fout << msk(ilon,ilat,0) << "\t"
						 << ft.lons[ilon] << "\t"
						 << ft.lats[ilat] << "\t"
						 << npp(ilon,ilat,0) << "\t";
					for (int ilev=0; ilev<ft.nlevs; ++ilev){
						fout << ft(ilon, ilat, ilev) << "\t";
					}
					fout << "\n";
				} 
			}
		}
		
		fout.close();
//		npp_avg.fill(0);
		cout << "wrote month " << imon+1 << "\n";

	}
	
	npp.closeNcInputStream();
	
	cout << "> Successfully wrote fire NC file!!\n";
}






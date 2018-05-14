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
g++ -I/usr/local/netcdf-c-4.3.2/include -I/usr/local/netcdf-cxx-legacy/include -I/home/jaideep/codes/FIRE_CODES/libgsm_v2/include -L/home/jaideep/codes/FIRE_CODES/libgsm_v2/lib -L/usr/local/netcdf-cxx-legacy/lib -o sl standardize_lons.cpp -l:libgsm.so.2 -lnetcdf_c++  
-----------------------------------------------------*/


int str2int(string s){
	istringstream sin;
	sin.str(s);
	int val;
	sin >> val;
	return val;
}




int main(int argc, char ** argv){

	float lon0 = 0.25, lonf =  359.75, lat0 = -89.75, latf = 89.75, dx = 0.5, dy = 0.5, dt0 = 1, dlev = 1;
	int nlons, nlats, nlevs = 1;
	//const float glimits_custom[4] = {lon0, lonf, lat0, latf};

	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);

	// create a grid limits vector for convenience
	float glimits[] = {0, 360, -90, 90};
	vector <float> glim(glimits, glimits+4);

	string filename = argv[1]; 
	
	gsm_info_on = true;
	gsm_debug_on = true;

	gVar fti;
	NcFile_handle fti_handle;
	fti_handle.open(filename+".nc", "r", glimits);
	fti_handle.readCoords(fti);
	fti_handle.readVarAtts(fti);
	fti.printGrid();
//	fti.printValues();	
	
	
	gVar fto;
	fto.copyMeta(fti);
	fto.lons = createCoord(lon0, lonf, dx, nlons);
	fto.nlons = nlons;
	fto.values.resize(fto.nlons*fto.nlats*fto.nlevs, 0);

	NcFile_handle fto_handle;
	fto_handle.open(filename+"_sl.nc","w", glimits);
	fto_handle.writeCoords(fto);
	NcVar* vVar = fto_handle.createVar(fto);
	fto_handle.writeTimeValues(fto);
	fto.printGrid();

	for (int t=0; t<fti.ntimes; ++t){

		fti_handle.readVar(fti,t);

		for (int ilon=0; ilon<fti.nlons; ++ilon){
			for (int ilat=0; ilat<fti.nlats; ++ilat){
				float xlon = fto.lons[ilon]; if (xlon > 180) xlon -= 360;
				float xlat = fto.lats[ilat];
				for (int ilev=0; ilev<fti.nlevs; ++ilev){
					fto(ilon,ilat,ilev) = fti.getCellValue(xlon, xlat, ilev);
				}
			}	
		}	

		fto_handle.writeVar(fto, vVar, t); // write data at time index ix
		cout << t << "\n";	
	}

	fto_handle.close();
	fti_handle.close();
		
	cout << "> Successfully wrote fire NC file!!\n";
}






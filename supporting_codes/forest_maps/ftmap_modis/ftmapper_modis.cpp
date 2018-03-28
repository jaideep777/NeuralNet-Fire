#include <iostream>
#include <gsm.h>
#include <netcdfcpp.h>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

// g++ -I/usr/local/netcdf-c-4.3.2/include -I/usr/local/netcdf-cxx-legacy/include -I/home/jaideep/codes/FIRE_CODES/libgsm_v2/include -L/home/jaideep/codes/FIRE_CODES/libgsm_v2/lib -L/usr/local/netcdf-cxx-legacy/lib -o 1 ftmapper_modis.cpp -l:libgsm.so.2 -lnetcdf_c++ 

int pfts[] = {	0	, //	water
				1	, //	evergreen needleleaf forest
				2	, //	evergreen broadleaf forest
				3	, //	deciduous needleleaf forest
				4	, //	deciduous broadleaf forest
				5	, //	mixed forests
				6	, //	closed shrublands
				7	, //	open shrublands
				8	, //	woody savannas
				9	, //	savannas
				10	, //	grasslands
				12	, //	croplands
				13	, //	urban and built-up
				16	, //	barren or sparsely vegetated
				254  //	unclassfied
			 };

int pftsNew[] = {	0	, //	water 
					1	, //	evergreen needleleaf forest (NLE)
					2	, //	evergreen broadleaf forest  (BLE)
					3	, //	deciduous needleleaf forest (NLD)
					4	, //	deciduous broadleaf forest  (BLD)
				   -1	, //	mixed forests				(MX)
					5	, //	closed shrublands
					6	, //	open shrublands
					7	, //	woody savannas
					8	, //	savannas
					9	, //	grasslands
					10	, //	croplands
					0	, //	urban and built-up
					0	, //	barren or sparsely vegetated
					0     //	unclassfied
				 };

int main(){
	
	// ~~~~~~ Some NetCDF Essentials ~~~~~~~~
	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);
	
	// specify log file for gsm
	ofstream gsml("gsm_log.txt");
	gsm_log = &gsml;

	// create a grid limits vector for convenience
	float glimits[] = {0, 360, -90, 90};
	vector <float> glim(glimits, glimits+4);
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~

	// create the coordinates for our georeferenced variable
	int nlons, nlats, nlevs, ntimes;
	vector <float> lats = createCoord_from_edges(5.0,50.0,0.5,nlats);
	vector <float> lons = createCoord_from_edges(60.0,100.0,0.5,nlons);
	vector <float> levs = createCoord(0,10,1,nlevs);
	vector <double> times(1); 
	times[0]=ymd2gday("2015-6-1")-ymd2gday("2000-1-1");

	cout << lats.size()  << ": "; printArray(lats);
	cout << lons.size()  << ": "; printArray(lons);

	gVar ftmodis;
	ftmodis.createOneShot("/media/jaideep/Totoro/Data/forest_type/MODIS/MOD12Q1_UMD_SASplus.nc", glim);
	ftmodis.printGrid();	

	gVar ft("ft", "-", "days since 2000-1-1");
	ft.setCoords(times, levs, lats, lons);
	ft.printGrid();	
	
	// create PFT map
	map<int,int> pft_map;
	for (int i=0; i<15; ++i) pft_map[pfts[i]] = pftsNew[i];

	int count =0;
	for (int ilat=0; ilat < ftmodis.nlats; ++ilat){
		for (int ilon=0; ilon < ftmodis.nlons; ++ilon){

			vector <int> uv = findGridBoxC(ftmodis.lons[ilon], ftmodis.lats[ilat], ft.lons, ft.lats);
			
			int modis_type = ftmodis(ilon, ilat, 0);

			if (uv[0] != -999 && uv[1] != -999) 
				if (modis_type != 5){	// is ft is mixed, add to NLE and MD
					ft(uv[0], uv[1], pft_map[modis_type]) += 0.0083333333333333*0.0083333333333333/0.5/0.5;
				}
				else{
					if (ftmodis.lats[ilat] > 26){	// is lat is > 26, add 50-50 to NLE and MD
						ft(uv[0], uv[1], pft_map[1]) += 0.5*0.0083333333333333*0.0083333333333333/0.5/0.5;
						ft(uv[0], uv[1], pft_map[8]) += 0.5*0.0083333333333333*0.0083333333333333/0.5/0.5;
					}
					else{							// else add only to MD
						ft(uv[0], uv[1], pft_map[8]) += 0.0083333333333333*0.0083333333333333/0.5/0.5;
					}
				}
			if (count %100000 ==0) cout << uv[0] << " " << uv[1] << " " << ftmodis.lats[ilat] << " " << ftmodis.lons[ilon] << " " << ftmodis(ilon, ilat, 0) << "\n";

			++count;
		}
	}
	
	ft.writeOneShot("ftmap_modis_new_11levs_noMixed.nc");
	
	return 0;

}



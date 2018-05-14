#include <iostream>
#include <gsm.h>
#include <netcdfcpp.h>
#include <vector>
using namespace std;

// g++ -I/usr/local/netcdf-c-4.3.2/include -I/usr/local/netcdf-cxx-legacy/include -I/home/jaideep/codes/FIRE_CODES/libgsm_v2/include -L/home/jaideep/codes/FIRE_CODES/libgsm_v2/lib -L/usr/local/netcdf-cxx-legacy/lib -o 1 dft.cpp -l:libgsm.so.2 -lnetcdf_c++ 

template <class T> 
int max_index(vector <T> v){
	T max = v[0]; int imax = 0;
	for (int i=1; i<v.size(); ++i){
		if (v[i] > max) {
			imax = i;
			max = v[i];
		}
	}
	return imax;
}

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

	// model grid
//	int nlons, nlats;
//	vector <float> mglons = createCoord(66.5,100.5,0.5, nlons);	
//	vector <float> mglats = createCoord(6.5,38.5,0.5, nlats);	

	// create the georeferenced variable
	gVar v;
	v.createOneShot("ftmap_modis_new_11levs_noMixed.nc", glim);
		
//	gVar m;
//	m.createOneShot("/media/jaideep/WorkData/Fire_G/util_data/masks/surta_india_0.2.nc", glim);
//	gVar m1 = coarseGrain_mean(m, v.lons, v.lats);
	
	gVar w;
	w.copyMeta(v);
	w.levs = vector <float> (1,0); w.nlevs=1;
	w.values.resize(w.nlons*w.nlats*w.nlevs);

	gVar ftmask;
	ftmask.copyMeta(v);
	ftmask.levs = vector <float> (1,0); ftmask.nlevs=1;
	ftmask.values.resize(ftmask.nlons*ftmask.nlats*ftmask.nlevs);

	
	for (int ilat=0; ilat<v.nlats; ++ilat){
	for (int ilon=0; ilon<v.nlons; ++ilon){
//		if (m1(ilon, ilat, 0) < 0.5 || m1(ilon, ilat, 0) == m1.missing_value){
//			w(ilon,ilat,0) = w.missing_value;
//		}
//		else{
			float ff = 1 - v(ilon, ilat, 0) - v(ilon, ilat, 10); // forest frac = 1- barren(0) - agri(10)
			
			if (v(ilon, ilat, 10) > 0.5) w(ilon,ilat,0) = 10;	// set agri as PFT if it is > 50%
//			else w(ilon,ilat,0) = 0;
			else if (v(ilon, ilat, 0) > 0.5) w(ilon,ilat,0) = 0;  // set X as pft is ut is > 50%
			else{												// else treat as forest area
				vector <float> ffs(v.nlevs);
				for (int i=0; i<ffs.size(); ++i) ffs[i] = v(ilon, ilat, i);
				ffs[0] = ffs[10] = 0; // set agri and barren fracs to 0 (to find max among forest types)
				int first_id = max_index(ffs);
				float first = ffs[first_id];
				ffs[first_id] = 0;
				int second_id = max_index(ffs);
				float second = ffs[second_id];
				if ((first - second) > 0.1) w(ilon, ilat, 0) = first_id; // if top PFT is at least 10% more than second, assign as dominant
				else w(ilon,ilat,0) = 11;								 // else classify as mixed
			}

			if (ff > 0.3) ftmask(ilon,ilat,0) = 1;
			else ftmask(ilon,ilat,0) = 0;
			
//			w(ilon,ilat,0) = ? 1:0;
//			w(ilon,ilat,0) += (ff > 0.3)? 1:0;
//			w(ilon,ilat,1) = max_pft2;
//			w(ilon,ilat,2) = highest - second;
//		}
	}
	}


	w.writeOneShot("dft_MODIS11lev_agri-bar_lt0.5_0.5deg.nc");
	ftmask.writeOneShot("ftmask_MODIS_0.5deg.nc");
	
//	gVar m2 = binary(coarseGrain_mean(m1, mglons, mglats), 0.3);
//	m2.writeOneShot("mask_india_0.5.nc");
	


	
	return 0;

}



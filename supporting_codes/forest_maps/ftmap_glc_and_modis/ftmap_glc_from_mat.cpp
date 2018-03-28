#include <iostream>
#include <math.h>
#include <netcdfcpp.h>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

#include "/home/jaideep/codes/libgsm/include/gsm.h"

/* --------------------------------------------------
compile command:
g++ -I/usr/local/netcdf-c-4.3.2/include -I/usr/local/netcdf-cxx-legacy/include -L/home/jaideep/codes/libgsm/lib -L/usr/local/netcdf-cxx-legacy/lib -o ftmap ftmap_glc_from_mat.cpp -lgsm -lnetcdf_c++
-----------------------------------------------------*/


int str2int(string s){
	istringstream sin;
	sin.str(s);
	int val;
	sin >> val;
	return val;
}

//int mapFireEvent(gVar &v, float xlon, float xlat){
//	int ilon = indexC(v.lons, xlon);
//	int ilat = indexC(v.lats, xlat);
//	if (ilon < 0 || ilat < 0) return 1;	// skip invalid lats/lons
//	int nlons = v.nlons, nlats = v.nlats, nlevs = v.nlevs;
//	v(ilon, ilat, 0) += 1;
//	return 0;
//}

int main(){

	float lon0 = -179.75, lonf =  179.75, lat0 = -89.75, latf = 89.75, dx = 0.5, dy = 0.5, dt0 = 1, dlev = 1;
	int nlons, nlats, nlevs = 1;
	//const float glimits_custom[4] = {lon0, lonf, lat0, latf};

	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);

	string basedate = "2009-01-01";
//	string startdate = year + "-1-1", enddate = year + "-12-31";
	double gt0 = ymd2gday(basedate);//, gtf = ymd2gday(enddate);
	double gtbase = ymd2gday(basedate);
	int ntimes = 1; //gtf-gt0+1;

	vector <float> lons = createCoord(lon0, lonf, dx, nlons);
	vector <float> lats = createCoord(lat0, latf, dx, nlats);
	vector <float> levs = createCoord(1, 23, 1, nlevs);
	vector <double> times(1, gt0-gtbase);
	vector <float> data(nlons*nlats*nlevs, 0);
//	for (int i=0; i<ntimes; ++i) times[i] = gt0 - gtbase + i;

	gVar ft("ftype", "--", "days since " + basedate);
	ft.setCoords(times, levs, lats, lons);
	ft.printGrid();
	ft.values = data;	


	ifstream fin; fin.open("/media/jaideep/totoro/Work/Data/Fire-Data/GLC/Tiff/data1.txt");	

	NcFile_handle ft_handle;
	ft_handle.open("ftmap_GLC_0.5.nc","w", glimits_globe);
	ft_handle.writeCoords(ft);
	NcVar* vVar = ft_handle.createVar(ft);
	ft_handle.writeTimeValues(ft);

//	float xlon, xlat, gt; string dat;
//	float xlon1, xlat1, gt1; string dat1;

//	while (fin >> xlat1){	// loop until 1st date in given range is found
//		fin >> xlon1 >> dat1;
//		gt1 = ymd2gday(dat1);
//		if (gt1 >= gt0) break;
//	}

	

	string s;
	getline(fin, s, '\n');	// header line 

	int count = 0;
//	float lat, lon, latc, lonc, oid, fid, sum;
//	vector <float> frac(23);
	int val;
	//40320 16353
	for (int iy=0; iy<16353; ++iy){
		for (int ix=0; ix<40320; ++ix){
//			getline(fin, s, ',');
//			int val = str2int(s);
			fin >> val;
			
			float lonc = -180 + ix*0.00892857140000;
			float latc = 89.99107138060005 -iy*0.00892857140000;
			
			int ilon = indexC(ft.lons, lonc);
			int ilat = indexC(ft.lats, latc);
		
			ft(ilon, ilat, val-1) += 1;
		
//			cout << latc << " " << lonc << ": ";
//			for (int i=0; i<23; ++i) cout << frac[i] << " ";
//			cout << sum << '\n';
		
//			count ++;
//			if (count % 1000 == 0) cout << "count = " << count << endl;
		}
		if (iy %100 ==0) cout << "iy = " << iy << endl;
	}

	ft_handle.writeVar(ft, vVar, 0); // write data at time index ix

//	setZero(fire.values);
//	int count = 0, cumcount = 0;
//	while (1){
//		int ix = fire.gt2ix(gt1);
//		mapFireEvent(fire, xlon1, xlat1); ++count;
//	
//		while (fin >> xlat){	// same time, so continue reading
//			fin >> xlon >> dat;
//			gt = ymd2gday(dat);
//			if (gt != gt1){		// new time found, so break.
//				//timevals.push_back(fprev.time);
//				xlon1 = xlon; xlat1 = xlat; dat1 = dat; gt1 = gt;	// fprev = f
//				break;
//			} 
//			mapFireEvent(fire, xlon, xlat); /*f.printEvent()*/; ++count;
//		}
//		
//		// all events mapped for time gt. write the data cube to NC file and empty it for next timestep
//		cout << "> Writing " << count << "\trecords for date = " 
//			 << gday2ymd(fire.ix2gt(ix)) << ", index = " << ix << '\n';
//		cumcount += count;
//		fire_handle.writeVar(fire, vVar, ix); // write data at time index ix
//		//cout << "after put\n";
//		setZero(fire.values); // empty data matrix
//		//cout << "after setZero\n";

//		if (gt > gtf) {cout << "> last date detected!\n"; break;} // last f was an eof()
//		count =0;
//	}
//	

	cout << "> Successfully wrote fire NC file!!\n";
}


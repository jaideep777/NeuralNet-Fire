#include <iostream>
#include <math.h>
#include <netcdfcpp.h>
#include <fstream>
#include <vector>
using namespace std;

#include "/home/jaideep/libgsm/include/gsm.h"

int mapFireEvent(gVar &v, float xlon, float xlat){
	int ilon = indexC(v.lons, xlon);
	int ilat = indexC(v.lats, xlat);
	if (ilon < 0 || ilat < 0) return 1;	// skip invalid lats/lons
	int nlons = v.nlons, nlats = v.nlats, nlevs = v.nlevs;
	v(ilon, ilat, 0) += 1;
	return 0;
}

int main(){

	float lon0 = 66.5, lonf =  100.5, lat0 = 6.5, latf = 38.5, dx = 0.5, dy = 0.5, dt0 = 1, dlev = 1;
	int nlons, nlats, nlevs = 1;
	//const float glimits_custom[4] = {lon0, lonf, lat0, latf};

	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);

	string year;
	cout << "Enter year: ";
	cin >> year;
	cout << "\nEntered year: \"" << year << "\"";

	string basedate = "2000-01-01";
	string startdate = year + "-1-1", enddate = year + "-12-31";
	double gt0 = ymd2gday(startdate), gtf = ymd2gday(enddate);
	double gtbase = ymd2gday(basedate);
	int ntimes = gtf-gt0+1;

	vector <float> lons = createCoord(lon0, lonf, dx, nlons);
	vector <float> lats = createCoord(lat0, latf, dx, nlats);
	vector <double> times(ntimes);
	vector <float> levs(1,1);
	vector <float> data(nlons*nlats, 0);
	for (int i=0; i<ntimes; ++i) times[i] = gt0 - gtbase + i;

	gVar fire("fire", "events/day", "days since " + basedate);
	fire.setCoords(times, levs, lats, lons);
	fire.printGrid();
	fire.values = data;	
	
//	for (int i=0; i<ntimes; ++i) cout << fire.times[i] << ". " << gdatetime(fire.ix2gday(i)) << '\n';
	cout << "t0 = " << gt2string(fire.ix2gt(0)) << ", tf = " << gt2string(fire.ix2gt(ntimes-1)) << '\n';

	ifstream fin; fin.open("/media/WorkData/Fire/fire_modis_0.1/fire_events-2000-2011.txt");	

	NcFile_handle fire_handle;
	fire_handle.open("/media/WorkData/Fire/fire_modis_0.5/fire_events."+year+".nc","w", glimits_india);
	fire_handle.writeCoords(fire);
	NcVar* vVar = fire_handle.createVar(fire);
	fire_handle.writeTimeValues(fire);

	float xlon, xlat, gt; string dat;
	float xlon1, xlat1, gt1; string dat1;

	while (fin >> xlat1){	// loop until 1st date in given range is found
		fin >> xlon1 >> dat1;
		gt1 = ymd2gday(dat1);
		if (gt1 >= gt0) break;
	}

	setZero(fire.values);
	int count = 0, cumcount = 0;
	while (1){
		int ix = fire.gt2ix(gt1);
		mapFireEvent(fire, xlon1, xlat1); ++count;
	
		while (fin >> xlat){	// same time, so continue reading
			fin >> xlon >> dat;
			gt = ymd2gday(dat);
			if (gt != gt1){		// new time found, so break.
				//timevals.push_back(fprev.time);
				xlon1 = xlon; xlat1 = xlat; dat1 = dat; gt1 = gt;	// fprev = f
				break;
			} 
			mapFireEvent(fire, xlon, xlat); /*f.printEvent()*/; ++count;
		}
		
		// all events mapped for time gt. write the data cube to NC file and empty it for next timestep
		cout << "> Writing " << count << "\trecords for date = " 
			 << gday2ymd(fire.ix2gt(ix)) << ", index = " << ix << '\n';
		cumcount += count;
		fire_handle.writeVar(fire, vVar, ix); // write data at time index ix
		//cout << "after put\n";
		setZero(fire.values); // empty data matrix
		//cout << "after setZero\n";

		if (gt > gtf) {cout << "> last date detected!\n"; break;} // last f was an eof()
		count =0;
	}
	

	cout << "> Successfully wrote fire NC file!!\n";
}


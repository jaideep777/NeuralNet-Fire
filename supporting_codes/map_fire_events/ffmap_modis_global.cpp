#include <iostream>
#include <math.h>
#include <netcdfcpp.h>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

#include "/home/jaideep/codes/libgsm/include/gsm.h"

/* --------------------------------------------------
compile command:
g++ -I/usr/local/netcdf-c-4.3.2/include -I/usr/local/netcdf-cxx-legacy/include -L/home/jaideep/codes/libgsm/lib -L/usr/local/netcdf-cxx-legacy/lib -o ffmap_g ffmap_modis_global.cpp -lgsm -lnetcdf_c++
-----------------------------------------------------*/

class fireEvent{
	public:
	float lon, lat;
	string date;
};

fireEvent parse(string s){
	replace(s.begin(), s.end(), ',', ' ');
	istringstream sin; sin.str(s);
	float lat,lon,brightness,scan,track,acq_time,confidence,version,bright_t31,frp;
	string acq_date, satellite;
	sin >> lat >> lon >> brightness >> scan >> track >> acq_date >> acq_time >> satellite >> confidence >> version >> bright_t31 >> frp;    
//	cout << " " <<  lat << " " <<  lon << " " <<  brightness << " " <<  scan << " " <<  track << " " <<  acq_date << " " <<  acq_time << " " <<  satellite << " " <<  confidence << " " <<  version << " " <<  bright_t31 << " " <<  frp << endl;    
	fireEvent f;
	f.lat = lat; f.lon = lon; f.date = acq_date; 
	return f;
}


int mapFireEvent(gVar &v, fireEvent e){
	int ilon = indexC(v.lons, e.lon);
	int ilat = indexC(v.lats, e.lat);
	if (ilon < 0 || ilat < 0) return 1;	// skip invalid lats/lons
	v(ilon, ilat, 0) += 1;
//	cout << "\tmap entry @ (" << v.lons[ilon] << ", " << v.lats[ilat] << ") on " << e.date << "\n";
	return 0;
}


int main(int argc, char ** argv){

	float lon0 = -179.75, lonf =  179.75, lat0 = -89.75, latf = 89.75, dx = 0.5, dy = 0.5, dt0 = 1, dlev = 1;
	int nlons, nlats, nlevs = 1;
	//const float glimits_custom[4] = {lon0, lonf, lat0, latf};

	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);

	string year;
	if (argc < 2){
		cout << "Enter year: ";
		cin >> year;
	}
	else year = argv[1];
	cout << "\nEntered year: \"" << year << "\"\n";

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

	string prefix; // = "firms179231421243172";
	if (argc < 3) {
		cout << "ERROR: Required filename_prefix. It is of the form 'firms179231421243172' followed by _\n";
		return 1;
	}
	else{
		prefix = argv[2];
	}
	
	string dir = "/media/jaideep/totoro/Work/Data/Fire-Data/fire_events_modis/global";
	string file = prefix+"_MCD14ML/"+prefix+"1_MCD14ML";
	string fname = dir + "/"+ file + "_sorted.csv";
	ifstream fin; fin.open(fname.c_str());	
	if (!fin) {
		cout << "ERROR: could not open file\n";
		return 1;
	}

	NcFile_handle fire_handle;
	fire_handle.open(dir+"/fire_events."+year+".nc","w", glimits_india);
	fire_handle.writeCoords(fire);
	NcVar* vVar = fire_handle.createVar(fire);
	fire_handle.writeTimeValues(fire);

	string fireLine;
	getline(fin, fireLine, '\n');	// header

	setZero(fire.values);
	int count = 0, cumcount = 0;

	ofstream lfout; 
	lfout.open(string(dir+"/"+prefix+"_MCD14ML/log_"+year+".txt").c_str());

	fireEvent enow, edaystart;
//	eprev.date = "";	// make sure now is not equal to prev in first reading

	bool startFlag = true;

	while (!fin.eof()){
	
		// read new event
		getline(fin, fireLine, '\n');	// header
		if (!fin.eof()) enow = parse(fireLine);
		else enow.date = "";

		if (startFlag) {	// for first entry, continue.. daystart is not mapped yet
			startFlag = false;
			edaystart = enow;
			continue;
		}
	
		if (enow.date == edaystart.date){	// same date, so map event and continue to read next event
			// map previous entry
			mapFireEvent(fire, enow);
			++count;
			continue; // skip rest and continue to reading next
		}

		// above if not entered, so new date found
		mapFireEvent(fire, edaystart); ++count; 	// map the day start event which was never mapped
		
		// write record corresponding to edaystart date
		int ix = fire.gt2ix(ymd2gday(edaystart.date));
		cout << "> Writing " << count << "\trecords for date = " 
			 << gday2ymd(fire.ix2gt(ix)) << ", index = " << ix << '\n';
		lfout << "> Writing " << count << "\trecords for date = " 
			 << gday2ymd(fire.ix2gt(ix)) << ", index = " << ix << '\n';
		// all events mapped for time gt. write the data cube to NC file and empty it for next timestep
		fire_handle.writeVar(fire, vVar, ix); // write data at time index ix
		setZero(fire.values); // empty data matrix

		cumcount += count; count = 0;
		edaystart = enow;	// update previous entry 
		
	}
	
	cout << "total records written: " << cumcount << endl; 
	lfout << "total records written: " << cumcount << endl; 

	cout << "> Successfully wrote fire NC file!!\n";
}


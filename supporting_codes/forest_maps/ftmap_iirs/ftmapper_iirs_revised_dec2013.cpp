#include <iostream>
#include <math.h>
#include <netcdfcpp.h>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

#include "/home/jaideep/libgsm/include/gsm.h"

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

/*
forest types new:
0 = no vegetation
1 = agriculture
2 = Evergreen NL
3 = Evergreen BL
4 = Moist Deciduous 
5 = Dry Deciduous
6 = grass
7 = evergreen scrub
8 = deciduous scrub
*/

const int x_pft_code	= 0;
const int agr_pft_code  = 1;
const int nle_pft_code 	= 2;
const int ble_pft_code 	= 3;
const int md_pft_code 	= 4;
const int dd_pft_code 	= 5;
const int gr_pft_code 	= 6;
const int sc_pft_code 	= 7;


vector <int> vt(256,0);

int init_vt(){
	// BARREN
	// ---> befault vt is zero, any unclassified is set to barren (zero).
	// AGR
	vt[170] = agr_pft_code;
	vt[150] = agr_pft_code;
	vt[106] = agr_pft_code;
	vt[173] = agr_pft_code;
	vt[109] = agr_pft_code;
	vt[102] = agr_pft_code;
	vt[49] = agr_pft_code;
	vt[151] = agr_pft_code;
	vt[90] = agr_pft_code;
	vt[157] = agr_pft_code;
	vt[107] = agr_pft_code;
	vt[93] = agr_pft_code;
	vt[96] = agr_pft_code;
	vt[122] = agr_pft_code;
	// NLE
	vt[19] = nle_pft_code;
	vt[31] = nle_pft_code;
	vt[41] = nle_pft_code;
	vt[45] = nle_pft_code;
	vt[32] = nle_pft_code;
	vt[18] = nle_pft_code;
	vt[42] = nle_pft_code;
	// BLE
	vt[11] = ble_pft_code;
	vt[12] = ble_pft_code;
	vt[44] = ble_pft_code;
	vt[38] = ble_pft_code;
	vt[78] = ble_pft_code;
	vt[81] = ble_pft_code;
	// MD
	vt[23] = md_pft_code;
	vt[25] = md_pft_code;
	vt[36] = md_pft_code;
	vt[22] = md_pft_code;
	vt[16] = md_pft_code;
	vt[24] = md_pft_code;
	vt[30] = md_pft_code;
	// DD
	vt[26] = dd_pft_code;
	vt[27] = dd_pft_code;
	vt[29] = dd_pft_code;
	vt[28] = dd_pft_code;
	vt[37] = dd_pft_code;
	vt[128] = dd_pft_code;
	vt[54] = dd_pft_code;
	vt[92] = dd_pft_code;
	vt[53] = dd_pft_code;
	vt[124] = dd_pft_code;
	vt[94] = dd_pft_code;
	vt[73] = dd_pft_code;
	// GR
	vt[139] = gr_pft_code;
	vt[135] = gr_pft_code;
	vt[138] = gr_pft_code;
	vt[40] = gr_pft_code;
	vt[116] = gr_pft_code;
	vt[145] = gr_pft_code;
	vt[144] = gr_pft_code;
	vt[117] = gr_pft_code;
	vt[137] = gr_pft_code;
	vt[136] = gr_pft_code;
	vt[141] = gr_pft_code;
	vt[21] = gr_pft_code;
	vt[147] = gr_pft_code;
	// SC	
	vt[120] = sc_pft_code;
	vt[123] = sc_pft_code;
	vt[121] = sc_pft_code;
	vt[127] = sc_pft_code;
	vt[126] = sc_pft_code;
	vt[129] = sc_pft_code;
	vt[132] = sc_pft_code;
			
}

int main(){

	const int npfts = 8;

	float lon0 = 66.5, lonf =  100.5, lat0 = 6.5, latf = 38.5, dx = 0.25, dy = 0.25, dt0 = 1, dlev = 1;
	int nlons, nlats, nlevs = npfts;
	//const float glimits_custom[4] = {lon0, lonf, lat0, latf};

	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);

	string year = "2009", basedate = "2009-01-01";
	string startdate = year + "-1-1", enddate = year + "-12-31";
	double gt0 = ymd2gday(startdate), gtf = ymd2gday(enddate);
	double gtbase = ymd2gday(basedate);
	int ntimes = gtf-gt0+nle_pft_code;

	ifstream fin; fin.open("/media/WorkData/Fire/ftmap_iirs/india_forestclasses_0.25deg.txt");	
	string trash; float missingval;
	fin >> trash >> nlons;
	fin >> trash >> nlats;
	fin >> trash >> lon0;
	fin >> trash >> lat0;
	fin >> trash >> dx;
	fin >> trash >> dy;
	lonf = lon0 + (nlons-1)*dx;
	latf = lat0 + (nlats-1)*dy;
	
	vector <float> lons = createCoord(lon0, lonf, dx, nlons);
	vector <float> lats = createCoord(lat0, latf, dy, nlats);
	vector <double> times(1,0);
	vector <float> levs = createCoord(0, nlevs-1, 1, nlevs);

	gVar veg("vegtype", "-", "days since " + basedate);
	veg.setCoords(times, levs, lats, lons);
	veg.printGrid();
	veg.values.resize(nlons*nlats*nlevs, 0);
	
	init_vt();
	
	NcFile_handle veg_handle;
	veg_handle.open("/media/WorkData/Fire/ftmap_iirs/ftmap_iirs_8ft_dec2013.nc","w", glimits_india);
	veg_handle.writeCoords(veg);
	NcVar* vVar = veg_handle.createVar(veg);
	veg_handle.writeTimeValues(veg);

	string s;
	while (fin >> s && s != "VALUE");
	getline(fin, s, '\n');	// ditch header
	cout << s << '\n';

	int value, gridID, FT;
	long long int count;
	int gridID_prev = -999;
	int loopnum = 0, ngrid = 0;
	while (1){
		
//		if (ngrid > 2) break;
//		if (loopnum > 10) break;

		fin >> value >> count >> gridID >> FT;
//		cout << value << " " << count << " " <<  gridID << " " <<  FT << '\n';

		int ilon, ilat, ilev;
		
		if (gridID != gridID_prev || fin.eof()){
			if (loopnum != 0){
				// veg_handle.writeVar(veg, vVar, 0);
				cout << "Read values for gridID = " << gridID_prev << ". Normalizing...\n";
				
				int gridCount = 0;
				for (int i = 0; i< veg.nlevs; ++i){
					gridCount += veg(ilon, ilat, i);
				}
				cout << "ncells in (" << lats[ilat] << "," << lons[ilon] << ") = " << gridCount << '\n';
				for (int i = 0; i< veg.nlevs; ++i){
					veg(ilon, ilat, i) /= gridCount;
				}
				// setValue(veg.values, 0);
				// veg.values.resize(0);
				// veg.values.resize(nlons*nlats*nlevs, veg.missing_value);
		
				if (fin.eof()){
					//veg_handle.close();
					break;
				}
				++ngrid;
			}
			gridID_prev = gridID;
		}

		// gridID = nlons*ilat + ilon
		ilat = gridID/nlons;
		ilon = gridID - nlons*ilat;
		ilev = vt[FT];
//		cout << ilat << ' ' << ilon << ' ' << ilev << '\n';

		veg(ilon, ilat, ilev) += count;
		cout << "map(" << lons[ilon] << "," << lats[ilat] << "," << ilev << ") = " 
			 << veg(ilon, ilat, ilev) << '\n';
		
		++loopnum;
		//cout << loopnum << " "<< mon << '\n';
		//if (mon > 2) break;
	}

	veg_handle.writeVar(veg, vVar, 0);

	cout << "> Successfully wrote veg NC file!!\n";
}



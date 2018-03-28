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

const int barren_pft_code           = 0;
const int agri_pft_code             = 1;
const int nlevergreen_pft_code 		= 2;
const int evergreen_pft_code 		= 3;
const int moistdeciduous_pft_code 	= 4;
const int drydeciduous_pft_code 	= 5;
const int grass_pft_code 			= 6;
const int evgrscrub_pft_code 		= 7;
const int deciscrub_pft_code 		= 7;


vector <int> vt(256,0);

int init_vt(){
	// agriculture
	vt[170] = agri_pft_code;
	vt[150] = agri_pft_code;
	vt[106] = agri_pft_code;
	vt[102] = agri_pft_code;
	vt[107] = agri_pft_code;
	vt[109] = agri_pft_code;
	vt[173] = agri_pft_code;
	// NL evergreen
	vt[19] = nlevergreen_pft_code;
	vt[31] = nlevergreen_pft_code;
	vt[41] = nlevergreen_pft_code;
	vt[45] = nlevergreen_pft_code;
	vt[18] = nlevergreen_pft_code;
	vt[42] = nlevergreen_pft_code;
	vt[96] = nlevergreen_pft_code;
	// BL evergreen (2)
	vt[11] = evergreen_pft_code;
	vt[12] = evergreen_pft_code;
	vt[32] = evergreen_pft_code;
	vt[157] = evergreen_pft_code;
	vt[93] = evergreen_pft_code;
	// BL moist deciduous (3)
	vt[23] = moistdeciduous_pft_code;
	vt[25] = moistdeciduous_pft_code;
	vt[36] = moistdeciduous_pft_code;
	vt[24] = moistdeciduous_pft_code;
	vt[38] = moistdeciduous_pft_code;
	vt[81] = moistdeciduous_pft_code;
		// these were deciduous, we are assuming moist deci
	vt[16] = moistdeciduous_pft_code;
	vt[37] = moistdeciduous_pft_code;
	vt[92] = moistdeciduous_pft_code;
	vt[44] = moistdeciduous_pft_code;
	vt[73] = moistdeciduous_pft_code;
	// BL dry deciduous (4)
	vt[26] = drydeciduous_pft_code;
	vt[27] = drydeciduous_pft_code;
	vt[28] = drydeciduous_pft_code;
	vt[54] = drydeciduous_pft_code;
	// grass (5)
	vt[139] = grass_pft_code;
	vt[135] = grass_pft_code;
	vt[138] = grass_pft_code;
	vt[40] = grass_pft_code;
	vt[145] = grass_pft_code;
	vt[128] = grass_pft_code;
	vt[30] = grass_pft_code;
	vt[144] = grass_pft_code;
	vt[117] = grass_pft_code;
	vt[137] = grass_pft_code;
	vt[136] = grass_pft_code;
	vt[141] = grass_pft_code;
	vt[143] = grass_pft_code;
	vt[147] = grass_pft_code;
	vt[148] = grass_pft_code;
	// evergreen scrub (6)
	vt[122] = evgrscrub_pft_code;
	vt[126] = evgrscrub_pft_code;
	vt[127] = evgrscrub_pft_code;
	vt[49] = evgrscrub_pft_code;
	// deciduous scrub (7)
	vt[116] = deciscrub_pft_code;
	vt[120] = deciscrub_pft_code;
	vt[123] = deciscrub_pft_code;
	vt[121] = deciscrub_pft_code;
	vt[129] = deciscrub_pft_code;
	vt[29] = deciscrub_pft_code;
	vt[132] = deciscrub_pft_code;
	vt[53] = deciscrub_pft_code;
	vt[94] = deciscrub_pft_code;
		
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
	double gt0 = gdays(startdate), gtf = gdays(enddate);
	double gtbase = gdays(basedate);
	int ntimes = gtf-gt0+nlevergreen_pft_code;

	ifstream fin; fin.open("/home/jaideep/fire/data/forest_type_iirs/india_forestclasses_0.25deg.txt");	
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
	veg_handle.open("ftmap_iirs_8ft.nc","w", glimits_india);
	veg_handle.writeCoords(veg);
	NcVar* vVar = veg_handle.createVar(veg);
	veg_handle.writeTimeValues(veg);

	string s;
	while (fin >> s && s != "VALUE");
	getline(fin, s, '\n');	// ditch header
	cout << s << '\n';

	int value, gridID, FT;
	long long int count;
	int gridID_prev = -nlevergreen_pft_code;
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
					gridCount += veg.values[ID(ilon, ilat, i)];
				}
				cout << "ncells in (" << lats[ilat] << "," << lons[ilon] << ") = " << gridCount << '\n';
				for (int i = 0; i< veg.nlevs; ++i){
					veg.values[ID(ilon, ilat, i)] /= gridCount;
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

		veg.values[ID(ilon, ilat, ilev)] += count;
		cout << "map(" << lons[ilon] << "," << lats[ilat] << "," << ilev << ") = " 
			 << veg.values[ID(ilon, ilat, ilev)] << '\n';
		
		++loopnum;
		//cout << loopnum << " "<< mon << '\n';
		//if (mon > 2) break;
	}

	veg_handle.writeVar(veg, vVar, 0);

	cout << "> Successfully wrote veg NC file!!\n";
}



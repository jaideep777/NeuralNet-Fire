#include <iostream>
#include <math.h>
#include <netcdfcpp.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
using namespace std;

#include "/home/jaideep/codes/libgsm/include/gsm.h"

/* --------------------------------------------------
compile command:
g++ -I/usr/local/netcdf-c-4.3.2/include -I/usr/local/netcdf-cxx-legacy/include -L/home/jaideep/codes/libgsm/lib -L/usr/local/netcdf-cxx-legacy/lib -o ftmap ftmap_reassign.cpp -lgsm -lnetcdf_c++  
-----------------------------------------------------*/


int str2int(string s){
	istringstream sin;
	sin.str(s);
	int val;
	sin >> val;
	return val;
}


/* ----- PFTs India ----------------------------------------------------

	0	X		barren / water / no vegetation
	1	AGR		agriculture
	2	BLE		broadleaved evergreen
	3	NLE		needleleaved evergreen
	4	MD		moist deciduous / closed deciduous / 
	5	DD		dry deciduous   /  open deciduous  / woody savanna 
	6	NLD		needleleaved deciduous
	7	GRA		grass / herb 
	8	SCE		evergreen shrub 
	9	SCD		deciduous shrub / savanna
	10	SCX		desert scrub / open shrub / sparse herb

---------------------------------------------------------------------*/

/* ----- PFTs Global ----------------------------------------------------

	0	X		 barren / water / no vegetation
	1	AGR		 agriculture
	2	BLE		 broadleaved evergreen
	3	NLE		 needleleaved evergreen
	4	BLD (DD) broadleaved deciduous / woody savannas 
	5	NLD		 needleleaved deciduous 
	6	GR		 needleleaved deciduous
	7	SCD		 savannas 
	8	SCX		 open shrublands, closed shrublands (dry thorn) 

---------------------------------------------------------------------*/



int npft = 9;
map <int, int> pftMap;

void createPftMap_GLC(){
	pftMap[7]  = 0;	// X
	pftMap[8]  = 0;
	pftMap[15] = 0;
	pftMap[19] = 0;
	pftMap[20] = 0;
	pftMap[21] = 0;
	pftMap[22] = 0;
	pftMap[23] = 0;
	pftMap[16] = 1;	// AGR
	pftMap[17] = 1;
	pftMap[18] = 1;
	pftMap[1] = 2;	// BLE
	pftMap[4] = 3;	// NLE
	pftMap[6] = 3;	
	pftMap[2] = 4;	// MD
	pftMap[3] = 5;	// DD
	pftMap[5] = 6;	// NLD
	pftMap[13] = 7;	// GR
	pftMap[11] = 8;	// SCE
	pftMap[12] = 9;	// SCD
	pftMap[9] = 9;
	pftMap[10] = 9; 
	pftMap[14] = 10; // SCX - Dry thorn
}

void createPftMap_MOD(){
	pftMap[0] = 0;	// x
	pftMap[13] = 0;
	pftMap[14] = 0;
	pftMap[15] = 0;
	pftMap[12] = 1;	// agr
	pftMap[2] = 2;	// ble
	pftMap[1] = 3;	// nle
	pftMap[5] = 3;	
//	pftMap[] = 4; // md
	pftMap[4] = 5; // dd
	pftMap[8] = 5; 
	pftMap[3] = 6;	// nld
	pftMap[10] = 7;	// gra
//	pftMap[] = 8;	// sce
	pftMap[9] = 9;	// scd
	pftMap[6] = 10;	// scx
	pftMap[7] = 10;	
}


void createPftMap_MOD_9PFT(){
	pftMap[0] = 0;	// x
	pftMap[13] = 0;
	pftMap[14] = 0;
	pftMap[15] = 0;
	pftMap[12] = 1;	// agr
	pftMap[2] = 2;	// ble
	pftMap[1] = 3;	// nle
	pftMap[5] = 3;	
//	pftMap[] = 4; // 
	pftMap[4] = 4; // bld
	pftMap[8] = 4; 
	pftMap[3] = 5;	// nld
	pftMap[10] = 6;	// gra
//	pftMap[] = 8;	// sce
	pftMap[9] = 7;	// scd
	pftMap[6] = 8;	// scx
	pftMap[7] = 8;	
}
	

int main(){

	float lon0 = -179.75, lonf =  179.75, lat0 = -89.75, latf = 89.75, dx = 0.5, dy = 0.5, dt0 = 1, dlev = 1;
	int nlons, nlats, nlevs = 1;
	//const float glimits_custom[4] = {lon0, lonf, lat0, latf};

	// set NETCDF error behavior to non-fatal
	NcError err(NcError::silent_nonfatal);

	gVar fti;
	NcFile_handle fti_handle;
	fti_handle.open("ftmap_MOD12q1_0.5.nc", "r", glimits_globe);
	fti_handle.readCoords(fti);
	fti_handle.readVarAtts(fti);
	fti_handle.readVar(fti,0);
	fti.printGrid();
//	fti.printValues();	
	
	createPftMap_MOD_9PFT();
	
	vector <float> levs = createCoord(0, npft-1, 1, nlevs);
	
	gVar fto;
	fto.shallowCopy(fti);
	fto.nlevs = nlevs;
	fto.levs = levs;
	fto.values.resize(fto.nlons*fto.nlats*fto.nlevs, 0);

	for (int ilon=0; ilon<fti.nlons; ++ilon){
		for (int ilat=0; ilat<fti.nlats; ++ilat){
			vector <float> levCounts(npft,0);
			for (int ilev=0; ilev<fti.nlevs; ++ilev){
				levCounts[pftMap[ilev]] += fti(ilon,ilat,ilev);	// pftMap[ilev+1] for GLC, pftMap[ilev] for MOD  
			}	
			
			float sum=0;
			for (int i=0; i<npft; ++i) sum += levCounts[i];
			if (sum > 0){
				for (int i=0; i<npft; ++i) levCounts[i] /= sum;
			}
			else{
				for (int i=0; i<npft; ++i) levCounts[i] = 0;
				levCounts[0]=1;
			}

			cout << "(" << fti.lons[ilon] << ", " <<  fti.lats[ilat] << "): " << sum << endl;
		
			for (int i=0; i<npft; ++i) fto(ilon,ilat,i) = levCounts[i];
		}	
	}	

	NcFile_handle fto_handle;
	fto_handle.open("ftmap_MOD12q1_0.5_9pft.nc","w", glimits_globe);
	fto_handle.writeCoords(fto);
	NcVar* vVar = fto_handle.createVar(fto);
	fto_handle.writeTimeValues(fto);
	fto_handle.writeVar(fto, vVar, 0); // write data at time index ix	
	
	cout << "> Successfully wrote fire NC file!!\n";
}






using namespace std;
#include "../include/ndr.h"
#include "../include/globals.h"


int calc_ndr_dayavg(double gt){ 

	int m = gt2month(gt);
//	int yr = gt2year(gt);

	float N = gt2daynum(gt);
	
	gVar ndr_t; 
	ndr_t.shallowCopy(ndr);
	ndr_t.values.resize(ndr_t.nlons*ndr_t.nlats);
	ndr_t.fill(0);
	
	float dt_t = 1;
	int n = 24/dt_t;
	for (float hr = 0; hr < 24; hr=hr+dt_t){ 

		float y = 2*pi/365*(N-1 + (hr-12)/24);

		float eqtime = 229.18*(0.000075 + 0.001868*cos(y) - 0.032077*sin(y)
					 - 0.014615*cos(2*y) - 0.040849*sin(2*y));
	
		float decl = 0.006918 - 0.399912*cos(y) + 0.070257*sin(y) - 0.006758*cos(2*y)
				   + 0.000907*sin(2*y) - 0.002697*cos(3*y) + 0.00148*sin(3*y);		// radians

		for (int ilat=0; ilat<mgnlats; ++ilat){
			for (int ilon=0; ilon<mgnlons; ++ilon){

				float lon = mglons[ilon];			// lon in deg
				float time_offset = eqtime + 4*lon; // + 60*5.5;	// 5.5 is timezone
				float tst = hr*60 + time_offset;
				float ha = (tst/4 - 180)*pi/180;	// radians

				float lat = mglats[ilat]*pi/180;	// lat in radians
				float sinY = sin(lat)*sin(decl)+ cos(lat)*cos(decl)*cos(ha); // cos(zenith)
			
//				float rhum = rh(ilon, ilat, 0)/100;
				float SW_d = 0;
				if (sinY > 0) {
					SW_d = 1370*(0.39 + 0.52*exp(-0.14/pow(sinY,1.13)));//*(1-pow(0/100,.5)/5);	// incoming shortwave 
//					SW_d = SW_d*(1-pow(rhum,0.5)/5);
					if (SW_d < 0) SW_d = 0;
					if (SW_d >1370) SW_d=1370;
				}

//				if (ilon == 20 && ilat == 20) cout << pr(ilon,ilat,0) << endl;
				
//				float cldfrac = cld(ilon,ilat,0)/100; //pr(ilon,ilat,0)/3e-4;
//				if (cld < 0) cld = 0; if (cld > 1) cld = 1;
				
				// downward shortwave
				ndr_t(ilon, ilat, 0) += sinY*SW_d/n; //*(1-0.17)*(1-cldfrac*0.6);// - 100; // 100 is lw_in - lw_out

			}
		}
	
	}
	ndr.copyValues(ndr_t);
	ndr.t = gt;
}	






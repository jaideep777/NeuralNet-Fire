using namespace std;
#include "../include/moisture.h"
#include "../include/globals.h"

#include <cmath>

//float rhobL = 10; 				// kg/m3 = gm/m2/mm
//float theta_sL = 0.8; 			// saturation water content of litter

//ofstream fout_mois("../output/mois_tease.txt");


int calc_ndr(double gt){ 

	int doy = gt2dayOfYear(gt);

	ndr.readVar_it(doy);
	cld.readVar_gt(gt+14,0);
	pr.readVar_gt(gt,0);
	rh.readVar_gt(gt,0);
	ts.readVar_gt(gt+14,0);	 // +14 for montly cru data
	wsp.readVar_gt(gt,0);
//	ffev.readVar_gt(gt,0);
	
	for (int ilat=0; ilat<mgnlats; ++ilat){
		for (int ilon=0; ilon<mgnlons; ++ilon){

			if (ndr(ilon,ilat,0) == ndr.missing_value){
				ndr(ilon,ilat,0) = ndr.missing_value;
			}
			else
			{

				float rhum = rh(ilon, ilat, 0);
				float ndr_net = ndr(ilon,ilat,0);			// downward SW radiation (unabsorbed)
				ndr_net *= (1-cld(ilon,ilat,0)/100*0.6); 		// absorption/reflection by clouds
				ndr_net = ndr_net*(1-pow(rhum/100,.5)/5);	// absorption by water vapour 
				ndr_net *= (1-albedo(ilon,ilat,0)/100);		// reflected by surface albedo
			
				ndr(ilon,ilat,0) = fmax(ndr_net - 100, 0); // 100 is lw_in - lw_out

			}

		}
	}

	ndr.t = gt;
		
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	--> calc_moisture(...)
	
	calculate LAI
	calculate radiation reaching the ground
	calculate evaporation rate
	solve bucket model to calculate moisture content
		
	Input: all weather variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int calc_moisture(){ 

	for (int ilat=0; ilat<mgnlats; ++ilat){
		for (int ilon=0; ilon<mgnlons; ++ilon){

			if( ndr(ilon,ilat,0) == ndr.missing_value 	|| 
				ps(ilon,ilat,0) == ps.missing_value 	|| 
				rh(ilon,ilat,0) == rh.missing_value 	|| 
				ts(ilon,ilat,0) == ts.missing_value 	|| 
				wsp(ilon,ilat,0) == wsp.missing_value 	||
				pr(ilon,ilat,0) == pr.missing_value  	||
				lmois(ilon,ilat,0) == lmois.missing_value  ||
				cmois(ilon,ilat,0) == cmois.missing_value  )
			{
				lmois(ilon,ilat,0) = lmois.missing_value;
				cmois(ilon,ilat,0) = cmois.missing_value;
			}
			else{
			
				// get all variables in the right units
				float Rn  = ndr(ilon,ilat,0)*0.0864;	// convert Rn from W/m2 to MJ/day/m2
				float T   = ts(ilon,ilat,0); // - 273.16;	// ts in degC
				float RH  = rh(ilon,ilat,0)/100;	 	// rh (0-1)
				float U   = wsp(ilon,ilat,0);			// wsp in m/s
				float Ps  = ps(ilon,ilat,0)/1000; 		// ps in kPa
				float dzL = dxl(ilon,ilat,0)/rhobL;		// dzL in mm (dxl is in gm/m2, rhobL is in kg/m3)
				float pre = pr(ilon,ilat,0);			// pr in mm/day

				// calculate PFT independent values needed for PER
				// float es = 0.6108*exp(17.27*T/(T+237.3));	// kPa, T in degC (tetens)
				// float m = 4098.17*es/(T+237.3)/(T+237.3);	// kPa/degC (tetens)
				float es = 0.13332*exp(21.07-5336/(T+273.16));	// kPa (Merva); 0.1333 converts mmHg to kPa
				float m = 5336*es/(T+273.16)/(T+273.16); 		// kPa/degC (Merva) 
				float lv = (2501 - 2.361*T)*1e-3; 				// MJ/Kg - latent heat of vapourization. Needs T in degC. REF: Polynomial curve fits to Table 2.1. R. R. Rogers; M. K. Yau (1989). A Short Course in Cloud Physics (3rd ed.). Pergamon Press. p. 16. ISBN 0-7506-3215-1.
				float y = 0.0016286*Ps/lv; 						// kPa/degC -  slope of vapour pressure curve
				float de = es*(1-RH);							// kPa	- vapour pressure deficit


				// calculate LAI and Radiation reaching ground (Rl)
				float Wc_sat = 1;
//				float Wc_sat_vec[] = {0,0,0.5,0.5,0.5,0.5,0.5};
				float Rl = 0, Rc = 0, lai = 0;
				for (int i=0; i<npft; ++i){

					// calculate LAI = canbio/canbiomax * LAImax
					if (canbio(ilon,ilat,i) < canbio_max(ilon,ilat,i))
						lai = LAImax[i]*canbio(ilon,ilat,i)/(canbio_max(ilon,ilat,i)+1e-6);	// avoid NaN
					else 
						lai = LAImax[i];

						
					if (i != agri_pft_code)
					Wc_sat += Wc_sat_vec[i]*lai*vegtype(ilon,ilat,i);	// calculate total water holding capacity of canopy

					// R = Watts reaching canopy of i'th PFT, will be intercepted by canopy
					//   = R(W/m2)* f * acell(m2)
					// then add all the R's and divide by acell to get avg Rn (W/m2) 
					// since acell appears in both num & den, set it to 1 (normalized)
					float R_i = Rn* vegtype(ilon,ilat,i);	

					// Radiation reaching ground from i'th PFT's canopy
					// beer-lambert law REF: ffmodel_2003_pual_swuf (they have used 0.5)
					float Rl_i = R_i *exp(-0.4*lai);	
					Rl += Rl_i;	// add to total.

				}
				// calculate radiation absorbed by canopy
				// canopy for all PFT's is treated alike
				Rc = Rn - Rl;	// radiation intercepted by canopy is that which does not reach litter layer

				// calculate potential evaporation rates from canopy and litter
				float Ep_l = (m*Rl + 6.43*y*(1+0.536*U)*de)/lv/(m+y);  // mm/day - potential evap rate (litter)
				float Ep_c = (m*Rc + 6.43*y*(1+0.536*U)*de)/lv/(m+y);  // mm/day - potential evap rate (canopy)

				// calculate Precipitation reaching ground = all pr where PFT is X, 0 otherwise
				// where forest is present, all pr is intercepted, then drained as per the water content
				float pr_l = vegtype(ilon,ilat,0)*pre;	// pr reaching litter = f_barren * pr
				float pr_c = pre - pr_l;				// pr intercepted by canopy
			
				// Water balance for canopy
				float Wc = cmois(ilon, ilat, 0);		// mm		
				float beta_c = Wc/(Wc_sat+1e-6);		// Wc_sat (mm = kg/m2)		
				Wc += (pr_c - Ep_c*beta_c)/24.0f*dt;	// mm/day * days/hr *dt = mm/hr*dt = mm (in this timestep)
				if (Wc < 0) Wc = 0;
				
				float Dc = 0; // drain from canopy in (mm/day) 
				if (Wc >= Wc_sat){
					Dc = (Wc - Wc_sat)*24.0/dt;
					Wc = Wc_sat;
				}
				
				// water balance for litter (depends on litter thickness)
				float S = lmois(ilon,ilat,0);
				float beta_l = 0.25*(1-cos(S*pi))*(1-cos(S*pi)); 
				float qnet_in = (pr_l + Dc - Ep_l*beta_l)/24.0f;	// mm/hr
				float q_drain = (S >=1 && qnet_in > 0)? qnet_in:0; // when layer becomes saturated, all excess water should drain out
				// solve for litter moisture
				if (dzL > 0.5) {	// use this tolerance to avoid division by zero. If litter layer is too thin, it will saturate instantly
					S += (qnet_in - q_drain)/dzL/theta_sL *dt;
				}				
				else {
					S = 0;
				}
				if (S < 0) S = 0;
				if (S > 1) S = 1;
				
				// update lmois and cmois
				lmois(ilon,ilat,0) = S;
				cmois(ilon,ilat,0) = Wc;
				//evap(ilon, ilat, 0) = Ep_l + Ep_c;

			}

		}	// ilon loop ends
	}	// ilat loop ends

	lmois.t = ndr.t;
	cmois.t = ndr.t;
}




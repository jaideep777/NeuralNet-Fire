# this file specifies the locations and name of all input files
# this line is comment (there MUST be a space after #)

> FORCING_DATA_DIRS

forcing_data_dir	/media/jaideep/WorkData/Fire_G
# var |	  dir
ts		ncep_20cen/temp_sfc
rh		ncep_20cen/rhum_nsfc
wsp		ncep_20cen/wsp_10m
pr		precip_iitm
npp		GPP_modis
ffev	fire_events_modis/india/fire_modis_0.5


> FORCING_VARIABLE_DATA
# name | unit 	|	prefix   	|	start_yr |	end_yr | nyrs/file 
ts		 K			air.sfc			2000		2015		1		
rh		 %			rhum.sig995		2000		2015		1		
wsp		 m/s		wsp.10m			2000		2015		1		
pr		 mm/day		pr.iitm0		2000		2015		9		
npp		 gC/m2/s	npp				2000		2013		14		
ffev	 f/day		fire_events		2000		2015		1		

# file name will be taken as "prefix.yyyy.nc" or "prefix.yyyy-yyyy.nc"
# value types: ins (instantaneous), sum, avg (not used as of now)
# time_interp_modes: auto, hold, lter (not used as of now)
#	hold = hold previous value till next value is available
#	lter = interpolate in-between values (using previous and next times)
#	auto = hold for avg variables, lter for ins variables, sum-conservative random for sum variables

> STATIC_INPUT_FILES
# var	|	file 
msk			util_data/masks/surta_india_0.2.nc
vegtype		forest_type/IIRS/netcdf/ftmap_iirs_8pft.nc
albedo		albedo/albedo.avg.2004.nc
elev		util_data/elevation/elev.0.5-deg.nc

> INITIAL_CONDITION
lmois		lmois_spin_end.2000-12-31.nc

> END



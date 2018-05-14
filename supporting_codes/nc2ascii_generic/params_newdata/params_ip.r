# this file specifies the locations and name of all input files
# this line is comment (there MUST be a space after #)

> FORCING_DATA_DIRS

forcing_data_dir	/media/jaideep/WorkData/Fire_G
# var |	  dir
ts		ncep_reanalysis/ts
rh		ncep_reanalysis/rhum
wsp		ncep_reanalysis/wsp
pr		precip_trmm/combined/reordered_dims

> FORCING_VARIABLE_DATA
# name | unit 	|	prefix   	|	start_yr |	end_yr | nyrs/file | nlevs | mode
ts		 K			air.sig995		2000		2015		1			1
rh		 %			rhum.sig995		2000		2015		1			1
wsp		 m/s		wsp.sig995		2000		2015		1			1	
pr		 mm/day		pr.trmm-perm	2000		2015		1			1


# file name will be taken as "prefix.yyyy.nc" or "prefix.yyyy-yyyy.nc"
# value types: ins (instantaneous), sum, avg (not used as of now)
# time_interp_modes: auto, hold, lter (not used as of now)
#	hold = hold previous value till next value is available
#	lter = interpolate in-between values (using previous and next times)
#	auto = hold for avg variables, lter for ins variables, sum-conservative random for sum variables

> STATIC_INPUT_FILES
# var	|	nlevs | file 
ftmask		1		forest_type/MODIS/ftmask_MODIS_0.5deg.nc
msk			1		util_data/masks/surta_global_0.5_sl.nc


> TIME
timestep 	daily
start_date	2007-1-1
start_time	0:0:0
end_date	2007-12-31
end_time	23:0:0	
dt			24
base_date	1950-1-1

> MODEL_GRID
lon0	60.25
lonf	99.75
lat0	5.25
latf	50.25
dlat	0.5
dlon	0.5

> OUTPUT_FILE
ascii_fwi.txt

> END



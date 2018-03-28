# this file specifies the locations and name of all input files
# this line is comment (there MUST be a space after #)

> FORCING_DATA_DIRS

forcing_data_dir	/media/jaideep/WorkData/Fire_G
# var |	  dir

> FORCING_VARIABLE_DATA
# name | unit 	|	prefix   	|	start_yr |	end_yr | nyrs/file | nlevs | mode

# file name will be taken as "prefix.yyyy.nc" or "prefix.yyyy-yyyy.nc"
# value types: ins (instantaneous), sum, avg (not used as of now)
# time_interp_modes: auto, hold, lter (not used as of now)
#	hold = hold previous value till next value is available
#	lter = interpolate in-between values (using previous and next times)
#	auto = hold for avg variables, lter for ins variables, sum-conservative random for sum variables

> STATIC_INPUT_FILES
# var	|	nlevs | file 
vegtype		8		forest_type/IIRS/netcdf/ftmap_iirs_8pft.nc
elev		1		util_data/elevation/elev.0.5-deg.nc
soilw_i		1 		CPC_soil_mois/soilw.intercept.2001-2010.nc
soilw_t		1 		CPC_soil_mois/soilw.trend.2001-2010.nc
pr_i		1		precip_imd_trend/pr.intercept.2001-2010.nc
pr_t		1		precip_imd_trend/pr.trend.2001-2010.nc
gpp_i 		1		GPP_modis/gpp.intercept.2001-2010.nc
gpp_t		1		GPP_modis/gpp.trend.2001-2010.nc
ld			20		land_degradation/ld_kar.nc

> TIME
timestep 	daily
start_date	2005-1-1
start_time	0:0:0
end_date	2005-1-1
end_time	23:0:0	
dt			24
base_date	1950-1-1

> MODEL_GRID
lon0	74.05
lonf	78.55
lat0	11.45
latf	18.55
dlat	0.1
dlon	0.1

> OUTPUT_FILE
ascii_npp.txt

> END



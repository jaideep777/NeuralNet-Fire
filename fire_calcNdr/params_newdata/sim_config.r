# this file specifies all simulation config parameters
# this line is comment (there MUST be a space after #)
# headings followed by ">" must be exactly as given here and in the same order

> TIME
# time is in GMT
# spinup MUST BE ENABLED for now
spinup		on			# (on/off) off implies IC will be supplied from file
spin_date0	2005-4-15	# time assumed to be 0:0:0. Ideal start date is Apr 1
sim_date0	2000-1-1
sim_t0		0:0:0		# GMT
sim_datef	2000-12-31
sim_tf		23:0:0
dt			24 # 730.5048			# hours
base_date	2000-1-1	# base time assumed to be 0:0:0
# spinup end date is automatically taken as 1 time step lesser than sim_date0_t0

> MODEL_GRID
lon0	0.25
lonf	359.75
lat0	-89.75
latf	89.75
dlat	.5
dlon	.5
# output params for single point (for testing/characterizing)
xlon	76.5	# 76 31 50
xlat	11.5	# 11 35 41
pointOutFile	../output/point_run.txt
SPout_on	1	# 0/1

> OUTPUT_VARIABLES
# var		nc	sp  <- write flags to nc file and to single point output
pr			0	1
rh			0	1
ts			0	1
wsp			0	1
npp			0	1
ffev		0	1

evap		0	0
ndr			1	1
ps			0	0
dxl			0	1
canbio		0	1
canbio_max	0	0
lmois		0	1
cmois		0	0


# for debugging purposes only.
# using this makes the code slightly tedious but immensely easy to change quickly
> VARS_TO_USE
pr			1
rh			1
ts			1
wsp			1
npp			1
ffev		1

evap		0
ndr			1
ps			1
dxl			1
canbio		1
canbio_max	1
lmois		1
cmois		1

> END


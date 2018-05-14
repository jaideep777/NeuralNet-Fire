
rm(list = ls())

# Simulation name ("" or "india" or "ssaplus" etc)
sim_name           <- "sas"

# Directories to the fire model folder
fire_dir           <- "/home/jaideep/codes/FIRE_CODES" # root directory for fire codes
tensorflow_act_dir <- "/home/jaideep/tensorflow"	# tensorflow virtual env dir
libgsm_dir         <- "/home/jaideep/codes/FIRE_CODES/libgsm_v2" # libgsm dir

# lib directories
ncxx_legacy_dir    <- "/usr/local/netcdf-cxx-legacy" 
netcdf_dir         <- "/usr/local/netcdf-c-4.3.2"
netcdf_cxx4        <- "/usr/local/netcdf-cxx4"
hdf5_dir           <- "/usr/local/hdf5"
hdf4_dir           <- "/usr/local/hdf4"

# lib paths
lib_paths          <- paste0("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:",
                             hdf5_dir,"/lib:",
                             hdf4_dir,"/lib:",
                             netcdf_cxx4,"/lib:",
                             libgsm_dir,"/lib:",
                             ncxx_legacy_dir,"/lib:",
                             netcdf_dir,"/lib")

# LIB and INC paths

lIBPATH            <-  paste0("LIBPATH = -L",ncxx_legacy_dir,"/lib"," -L",libgsm_dir,"/lib")
INCPATH            <-  paste0("INCPATH = -I",netcdf_dir,"/include"," -I",ncxx_legacy_dir,"/include")
INCPATH2           <-  paste0("INCPATH += -I",libgsm_dir,"/include")  

source(paste0(fire_dir,"/R_scripts/utils.R"))

system("gedit /home/jaideep/codes/FIRE_CODES/fire_tensorflow/nn_const_data_fire_v4.py")
setwd("/home/jaideep/codes/FIRE_CODES/fire_tensorflow/")
system(paste0("./runtf"))

suffix = ""
if (sim_name != "") suffix = paste0(suffix,"_",sim_name)
output_dir = paste0("output",suffix)


setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix))
system("sed -i -e 's/\\[/ /g' weights_ba.txt")
system("sed -i -e 's/\\]/ /g' weights_ba.txt")
system("sed -i -e 's/\\,/ /g' weights_ba.txt")

#### Stage 4 ####
# agrregate (evaluates NN on nc files to generate fire nc file)

system("gedit /home/jaideep/codes/FIRE_CODES/fire_aggregateData/src/main.cpp")

setwd(paste0(fire_dir,"/fire_aggregateData"))
system(paste0(lib_paths," && make && ./aggregate eval ", sim_name))






glimits = c(60.25,99.75,5.25,29.75)  # sas

setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix ))

system("mkdir figures")

model = "x-lmois"
library(ncdf4)
library(chron)

system("/usr/local/cdo-1.6.7/bin/cdo ifthen -sellonlatbox,60.25,99.75,5.25,29.75 /media/jaideep/WorkData/Fire_G/forest_type/MODIS/ftmask_MODIS_0.5deg.nc fire.2007-1-1-2015-12-31.nc fire_pred_masked.nc")

fire_pred = NcCreateOneShot(filename = "fire_pred_masked.nc", var_name = "fire", glimits = glimits)
fire_pred = NcClipTime(fire_pred, "2007-1-1", "2015-11-30")
fire_pred$data = fire_pred$data - 0.001
fire_pred$data[fire_pred$data < 0.00] = 0

fire_obs = NcCreateOneShot(filename = "fire_obs_masked_2007-2015.nc", var_name = "ba", glimits = glimits)
fire_obs = NcClipTime(fire_obs, "2007-1-1", "2015-11-30")
fire_obs$data = fire_obs$data/55.5e3/55.5e3

ts_pred = apply(X = fire_pred$data, FUN = function(x){sum(x, na.rm=T)}, MARGIN = 3)
ts_obs = apply(X = fire_obs$data, FUN = function(x){sum(x, na.rm=T)}, MARGIN = 3)
tmpcor = cor(ts_pred, ts_obs)

# p_obs1 = p_obs[which(obs_t >= "2007-1-1" & obs_t <= "2015-12-31")]/55.5/55.5e6
# t1 = obs_t[which(obs_t >= "2007-1-1" & obs_t <= "2015-12-31")]

ts_obs_yr = (tapply(X = ts_obs, INDEX = strftime(fire_obs$time, "%Y"), FUN = sum))
ts_pred_yr = (tapply(X = ts_pred, INDEX = strftime(fire_pred$time, "%Y"), FUN = sum))
tmpcor_yoy = cor(ts_pred_yr, ts_obs_yr)

# plot(ts_obs_yr~unique(strftime(obs_t, "%Y")), ylim=c(0,75))
# points(ts_pred_yr~unique(strftime(obs_t, "%Y")), type="l", col="blue")
# plot(ts_obs_yr~ts_pred_yr)

slice_pred = apply(X = fire_pred$data, FUN = function(x){mean(x, na.rm=T)}, MARGIN = c(1,2))*24
slice_pred[is.na(slice_pred)] = 0

slice_obs = apply(X = fire_obs$data, FUN = function(x){mean(x, na.rm=T)}, MARGIN = c(1,2))*24 
slice_obs[is.na(slice_obs)] = 0

spacor = cor(as.numeric(slice_pred), as.numeric(slice_obs))


# write.table(x = spacor, file = "spacor.txt", row.names = F, col.names = F)

cols = createPalette(c("black", "blue","green3","yellow","red"),c(0,25,50,100,1000), n = 1000)
cols = createPalette(c("black", "black", "black","blue","mediumspringgreen","yellow","orange", "red","brown"),c(0,0.2,0.5,1,2,5,10,20,50,100)*1000, n = 1000)
cols = createPalette(c("black", "blue4", "blue", "skyblue", "cyan","mediumspringgreen","yellow","orange", "red","brown"),c(0,0.2,0.5,1,2,5,10,20,50,100)*1000, n = 1000) #gfed 

png(filename = paste0("figures/all_seasons", "(",model,").png"),res = 300,width = 600*3,height = 460*3) # 520 for sasplus, india
layout(matrix(c(1,1,
                1,1,
                # 2,3,
                2,3,
                2,3), ncol=2,byrow = T))  # vertical
par(mar=c(4,5,3,1), oma=c(1,1,1,1), cex.lab=1.5, cex.axis=1.5)

plot(y=ts_obs, x=fire_obs$time, col="orange2", type="o", cex=1.2, lwd=1.5, xlab="", ylab="Burned area")
points(ts_pred, x= fire_pred$time, type="l", col="red", lwd=2)
mtext(cex = 1, line = .5, text = sprintf("Correlations: Temporal = %.2f, Spatial = %.2f", tmpcor, spacor))

# par(mfrow=c(1,2))

image(fire_pred$lon, fire_pred$lat, slice_pred, col = cols, zlim = c(0,1), xlab="Longitude",ylab = "Latitude")
mtext(cex = 1, line = .5, text = sprintf("Total BA = %.2f Mha", sum(slice_pred, na.rm=T)*55.5e3*55.5e3*0.0001/1e6))
# plot(shp, add=T)

image(fire_obs$lon, fire_obs$lat, slice_obs, col = cols, zlim = c(0,1),xlab = "Longitude",ylab = "Latitude")
mtext(cex = 1, line = .5, text = sprintf("Total BA = %.2f Mha", sum(slice_obs)*55.5e3*55.5e3*0.0001/1e6))
# plot(shp, add=T)

# image(x=), fire_obs$lat, slice_obs, col = cols, zlim = c(0,1),xlab = "Longitude",ylab = "Latitude")

# slice_gfed = apply(X = fire_gfed$data, FUN = sum, MARGIN = c(1,2))/9
# slice_gfed[ftmask$data == 0] = 0
# image(fire_gfed$lon,fire_gfed$lat,slice_gfed,col = cols,zlim = c(0,1),xlab = "Longitude",ylab = "Latitude",cex.lab=1.6)
# mtext(line = .5, text = sprintf("Total BA = %.2f", sum(slice_gfed)*55.5e3*55.5e3*0.0001/1e6))
# # plot(shp, add=T)

mtext(text = "All seasons",side = 3,line = 1,outer = T)
dev.off()



setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix ))
sim = "x-lmois-9"
system(paste0("mkdir ",sim))

system(paste0("mv figures y_predic* weights_ba.txt fire.2007-1-1-2015-12-31.nc fire_pred_masked.nc ",sim))





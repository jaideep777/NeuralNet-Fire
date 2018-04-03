
library(ncdf4)
library(chron)

glimits = c(60.25,99.75,5.25,49.75)
# glimits = c(66.75, 98.25, 6.75, 38.25)

setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix,"/figures" ))

ncpath <- paste0(fire_dir,"/fire_aggregateData/output",suffix,"/")
# ncpath <- "/home/jaideep/codes/FIRE_CODES_ssaplus_working/fire_aggregateData/output/"
ncname <- "fire.2007-1-1-2015-12-31"
ncfname <- paste(ncpath, ncname, ".nc", sep="")
fire_pred = NcCreateOneShot(filename = ncfname, var_name = "fire", glimits = glimits)
# fire_pred$month = as.numeric(strftime(fire_pred$time, format = "%m"))
fire_pred = NcClipTime(fire_pred, "2007-1-1", "2015-11-30")
fire_pred$data[fire_pred$data < 0.002] = 0

ncpath <- "/media/jaideep/WorkData/Fire_G/fire_BA"
# ncpath <- "/media/jaideep/WorkData/Fire_G/fire_BA_GFED"
ncname <- "burned_area_0.5deg.2001-2016.nc"
# ncname <- "GFED4.0_MQ_0.5deg.1995-2016.nc"
ncfname <- paste(ncpath, "/", ncname, sep="")
fire_obs = NcCreateOneShot(filename = ncfname, var_name = "ba", glimits = glimits)
fire_obs = NcClipTime(fire_obs, "2007-1-1", "2015-11-30")
# fire_obs$month = as.numeric(strftime(fire_obs$time, format = "%m"))
# lat_ids = which(fire_obs$lats >= 6.375 & fire_obs$lats < 38.625)
# lon_ids = which(fire_obs$lons >= 66.375 & fire_obs$lons < 100.625)
fire_obs$data = fire_obs$data/55.5e3/55.5e3

ncpath <- "/media/jaideep/WorkData/Fire_G/fire_BA_GFED"
ncname <- "GFED4.0_MQ_0.5deg.1995-2016.nc"
ncfname <- paste(ncpath, "/", ncname, sep="")
fire_gfed = NcCreateOneShot(filename = ncfname, var_name = "ba", glimits = glimits)
fire_gfed = NcClipTime(fire_gfed, "2007-1-1", "2015-11-30")


# ftmask = NcCreateOneShot(filename="/media/jaideep/WorkData/Fire_G/forest_type/IIRS/netcdf/ftmask_0.5deg.nc", var_name = "vegtype", glimits = glimits)
ftmask = NcCreateOneShot(filename="/media/jaideep/WorkData/Fire_G/forest_type/MODIS/ftmask_MODIS_0.5deg.nc", var_name = "ft", glimits = glimits)

f = function(x){
  x
}

# cols = createPalette(c("black", "blue","green3","yellow","red"),c(0,250,500,750,1000), n = 1000)
cols = createPalette(c("black", "blue","green3","yellow","red"),c(0,25,50,100,1000), n = 1000)
cols = createPalette(c("black", "blue4", "blue", "skyblue", "cyan","mediumspringgreen","yellow","orange", "red","brown"),c(0,0.2,0.5,1,2,5,10,20,50,100)*1000, n = 1000) #gfed
# cols = createPalette(c("black", "black", "black", "black","blue","mediumspringgreen","yellow","orange", "red","brown"),c(0,0.2,0.5,1,2,5,10,20,50,100)*1000, n = 1000)

library(rgdal)
shp <- readOGR(dsn = "/media/jaideep/WorkData/Fire_G/util_data/india_boundaries/india_st.shp")

diffuse = function(mat, D, nt){
  mat1 = mat
  for (t in 1:nt){
    for (i in 2:(nrow(mat)-1)){
      for (j in 2:(ncol(mat)-1)){
        mat[i,j] = mat1[i,j] + D*(mat1[i+1,j] + mat1[i,j+1] + mat1[i-1,j] + mat1[i,j-1] - 4*mat1[i,j])/4
      }
    }
    mat1 = mat
  }
  mat
}


seasons = list(FMAM = c(2,3,4,5), JJAS = c(6,7,8,9), ON = c(10,11,12))
names = c("summer", "monsoon", "postmonsoon_winter")
seasmonths = c("FMAM", "JJAS", "OND")

for (sea in 1:length(seasons)){
  png(filename = paste0(names[sea], "(",model,").png"),res = 300,width = 1200,height = 2700)
  # layout(matrix(c(1,2,3,4,5,6,7,8),2,4,byrow = F))  # horizontal
  layout(matrix(c(1,2,3,4,5,6,7,8),4,2,byrow = T))  # vertical
  par(mar=c(4,4,3,1), oma=c(1,2,6,2), cex.lab=3, cex.axis=1.5)
  for(i in seasons[[sea]]){
    # slice_pred<- fire_pred$data[,,i]
    slice_pred = apply(X = fire_pred$data[,,which(fire_pred$month == i)], FUN = mean, MARGIN = c(1,2))
    slice_pred[ftmask$data == 0] = 0
    image(fire_pred$lon,fire_pred$lat,slice_pred,col = cols,zlim = c(0,1), xlab="Longitude",ylab = "Latitude",cex.lab=1.6)
    mtext(cex = 0.8, line = .5, text = sprintf("Total BA = %.2f", sum(slice_pred)*55.5e3*55.5e3*0.0001/1e6))
    # plot(shp, add=T, col="white")
    
    slice_obs = apply(X = fire_obs$data[,,which(fire_obs$month == i)], FUN = mean, MARGIN = c(1,2))
    slice_obs = (diffuse(slice_obs, 0.1, 0))
    slice_obs[ftmask$data == 0] = 0
    image(fire_obs$lon,fire_obs$lat,slice_obs,col = cols,zlim = c(0,1),xlab = "Longitude",ylab = "Latitude",cex.lab=1.6)
    mtext(cex = 0.8, line = .5, text = sprintf("Total BA = %.2f", sum(slice_obs)*55.5e3*55.5e3*0.0001/1e6))
    # plot(shp, add=T, col="white")
    
  }
  mtext(text = seasmonths[sea],side = 3,line = 1,outer = T)
  dev.off()
}



#########

# fire_pred1 = fire_pred
# fire_pred1$data[fire_pred1$data < 0.002] = 0
# cols = createPalette(c("black", "black", "black","blue","mediumspringgreen","yellow","orange", "red","brown"),c(0,0.2,0.5,1,2,5,10,20,50,100)*1000, n = 1000)
# par(mfrow=c(1,2))
# image(apply(fire_pred$data, MARGIN = c(1,2), FUN = mean)*24, col=cols, zlim=c(0,1))
# image(apply(fire_obs$data, MARGIN = c(1,2), FUN = mean)*24, col=cols, zlim=c(0,1))
# 


# r2 = summary(lm(ts_obs_eval~ts_pred_eval))$adj.r.squared
# r = cor(ts_obs_eval, ts_pred_eval)
# add_label(0.00, 0.15, label = sprintf("R (eval) = %.2f", r), cex=1.5, pos=4)

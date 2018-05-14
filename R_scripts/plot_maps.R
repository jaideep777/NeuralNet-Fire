
library(ncdf4)
library(chron)

sim_name           <- "ssaplus"
suffix = ""
if (sim_name != "") suffix = paste0(suffix,"_",sim_name)
output_dir = paste0("output",suffix)

glimits = c(60.25,99.75,5.25,49.75)
# glimits = c(66.75, 98.25, 6.75, 38.25)
# glimits = c(60.25,99.75,5.25,29.75)

setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix ))

fire_pred = NcCreateOneShot(filename = "fire_pred_masked.nc", var_name = "fire", glimits = glimits)
fire_pred = NcClipTime(fire_pred, "2007-1-1", "2015-11-30")
fire_pred$data = fire_pred$data - 0.001
fire_pred$data[fire_pred$data < 0.00] = 0

fire_obs = NcCreateOneShot(filename = "fire_obs_masked_2007-2015.nc", var_name = "ba", glimits = glimits)
fire_obs = NcClipTime(fire_obs, "2007-1-1", "2015-11-30")
fire_obs$data = fire_obs$data/55.5e3/55.5e3


f = function(x){
  x
}

# cols = createPalette(c("black", "blue","green3","yellow","red"),c(0,250,500,750,1000), n = 1000)
cols = createPalette(c("black", "blue","green3","yellow","red"),c(0,25,50,100,1000), n = 1000)
cols = createPalette(c("black", "blue4", "blue", "skyblue", "cyan","mediumspringgreen","yellow","orange", "red","brown"),c(0,0.2,0.5,1,2,5,10,20,50,100)*1000, n = 1000) #gfed
# cols = createPalette(c("black", "black", "black", "black","blue","mediumspringgreen","yellow","orange", "red","brown"),c(0,0.2,0.5,1,2,5,10,20,50,100)*1000, n = 1000)

# library(rgdal)
# shp <- readOGR(dsn = "/media/jaideep/WorkData/Fire_G/util_data/india_boundaries/india_st.shp")

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


seasons = list(FMAM = c(2,3,4,5), JJAS = c(6,7,8,9), ON = c(10,11,12,1))
names = c("summer", "monsoon", "postmonsoon_winter")
seasmonths = c("FMAM", "JJAS", "ONDJ")

for (sea in 1:length(seasons)){
  png(filename = paste0("figures/", names[sea], "(",model,").png"),res = 300,width = 1200,height = 2700) # 2700 for ssaplus, india, 2200 for SAS
  # layout(matrix(c(1,2,3,4,5,6,7,8),2,4,byrow = F))  # horizontal
  layout(matrix(c(1,2,3,4,5,6,7,8),4,2,byrow = T))  # vertical
  par(mar=c(4,4,3,1), oma=c(1,2,6,2), cex.lab=3, cex.axis=1.5)
  for(i in seasons[[sea]]){
    # slice_pred<- fire_pred$data[,,i]
    slice_pred = apply(X = fire_pred$data[,,which(fire_pred$month == i)], FUN = function(x){mean(x, na.rm=T)}, MARGIN = c(1,2))
    # slice_pred[ftmask$data == 0] = 0
    slice_pred[is.nan(slice_pred)] = NA
    image(fire_pred$lon,fire_pred$lat,slice_pred,col = cols,zlim = c(0,1), xlab="Longitude",ylab = "Latitude",cex.lab=1.6)
    mtext(cex = 0.8, line = .5, text = sprintf("Total BA = %.2f", sum(slice_pred, na.rm=T)*55.5e3*55.5e3*0.0001/1e6))
    # plot(shp, add=T, col="white")
    
    slice_obs = apply(X = fire_obs$data[,,which(fire_obs$month == i)], FUN = function(x){mean(x, na.rm=T)}, MARGIN = c(1,2))
    # slice_obs = (diffuse(slice_obs, 0.1, 0))
    # slice_obs[ftmask$data == 0] = 0
    slice_obs[is.nan(slice_obs)] = NA
    image(fire_obs$lon,fire_obs$lat,slice_obs,col = cols,zlim = c(0,1),xlab = "Longitude",ylab = "Latitude",cex.lab=1.6)
    mtext(cex = 0.8, line = .5, text = sprintf("Total BA = %.2f", sum(slice_obs, na.rm=T)*55.5e3*55.5e3*0.0001/1e6))
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

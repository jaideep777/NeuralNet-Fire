## CREATE timeseries
library(ncdf4)
library(chron)

fire_dir = "/home/jaideep/codes/FIRE_CODES"

for (mod in c("full_logpopp10")){ #}, "xdxl", "xlmois", "xts", "xpop", "xrh", "xwsp", "xrh_lmois")){
  
  for (i in 8:8){
    model = paste0(mod, "_", i)
    
    setwd(paste0(fire_dir,"/fire_aggregateData/output_ssaplus" ))
    
    # system("mkdir figures")
    
    glimits = c(60.25,99.75,5.25,49.75)  # ssaplus
    # glimits = c(66.75, 98.25, 6.75, 38.25) # India
    # glimits = c(60.25,99.75,5.25,29.75)  # sas

    fire_pred = NcCreateOneShot(filename = "full_8/fire_pred_masked.nc", var_name = "fire", glimits = glimits)
    fire_pred = NcClipTime(fire_pred, "2007-1-1", "2015-11-30")
    #fire_pred = NcClipTime(fire_pred, "2008-1-1", "2010-12-31")
    fire_pred$data = fire_pred$data - 0.002
    fire_pred$data[fire_pred$data < 0.00] = 0

    cell_area = t(matrix(ncol = length(fire_pred$lons), data = rep(55.5e3*55.5e3*cos(fire_pred$lats*pi/180), length(fire_pred$lons) ), byrow = F ))
    
    fire_pred2 = NcCreateOneShot(filename = paste0(model,"/fire_pred_masked.nc"), var_name = "fire", glimits = glimits)
    fire_pred2 = NcClipTime(fire_pred2, "2007-1-1", "2015-11-30")
    #fire_pred2 = NcClipTime(fire_pred2, "2008-1-1", "2010-12-31")
    fire_pred2$data = fire_pred2$data - 0.002
    fire_pred2$data[fire_pred2$data < 0.00] = 0
    
    slice_pred = apply(X = fire_pred$data, FUN = function(x){mean(x, na.rm=T)}, MARGIN = c(1,2))*24
    slice_pred[is.na(slice_pred)] = 0

    slice_pred2 = apply(X = fire_pred2$data, FUN = function(x){mean(x, na.rm=T)}, MARGIN = c(1,2))*24
    slice_pred2[is.na(slice_pred2)] = 0

    slice_diff = (slice_pred2 - slice_pred)/(slice_pred+1e-6)*100

    cols = createPalette(c("blue4", "blue", "white", "orange", "red"),c(-100, -50, 0, 50, 100)*1000, n = 1000) #
    
    png(filename = paste0("diff", "(",model,").png"),res = 300,width = 435*3,height = 476*3) # 520 for sasplus, india, 460 for SAS 
    par(mar=c(4,5,3,1), oma=c(1,1,1,1), cex.lab=1.5, cex.axis=1.5)
    
    image(fire_pred$lon, fire_pred$lat, slice_diff, col = cols, zlim = c(-100,100), xlab="Longitude",ylab = "Latitude")
    # mtext(cex = 2, line = .5, text = "Dxl +10%")
    # plot(shp, add=T)
    

    dev.off()
    
  }
}	
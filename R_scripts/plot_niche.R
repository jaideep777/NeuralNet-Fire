# rm(list = ls())
# source(paste0(fire_dir,"/R_scripts/utils.R"))
#### PREDICTED FIRES - CALIBRATION ####

for (model_name in c("gfed_xcf")){
  for (iter in 9:9){
  
    model = paste0(model_name, "_", iter)
    
    # Simulation name ("" or "india" or "ssaplus" etc)
    sim_name           <- paste0("ssaplus", "/", model)
    
    # Directories to the fire model folder
    fire_dir           <- "/home/jaideep/codes/FIRE_CODES" # root directory for fire codes
    
    #### Init ####
    suffix = ""
    if (sim_name != "") suffix = paste0(suffix,"_",sim_name)
    output_dir = paste0("output",suffix)
    
    dataset = "eval"
    system("mkdir -p figures")
    
    # datf_train = read.fireData(dataset = "train", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
    # datf_eval = read.fireData(dataset = "eval", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
    # datf_test = read.fireData(dataset = "test", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
    
    # datf = rbind(datf_eval, datf_test, datf_train)
    
    datf = read.fireData_gfed(dataset = dataset, dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
    
    
    plot.niche = function(datf, name="", max.baclass=11){
      # png(filename = paste0("niche(",model,"_",name,").png"), width = 400*3, height = 500*3, res = 300)
      png(filename = paste0("niche(",model,"_",name,").png"), width = 512*3, height = 790*3, res = 300)
      
      par(mfrow = c(4,2), mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
      
      col.obs = rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0)
      col.pred = rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0)
      
      col.obs = colorRampPalette(colors = c("mediumspringgreen","red"))(100)[floor(datf$baclass/max.baclass*100)+1]
      col.pred = colorRampPalette(colors = c("mediumspringgreen","red"))(100)[floor(datf$baclass_pred/max.baclass*100)+1]
      
      plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=col.obs, xlab = "Fuel mass", ylab="Fuel Moisture")
      points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass/4, col=col.obs)
      plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=col.pred, xlab = "Fuel mass", ylab="Fuel Moisture")
      points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass_pred/4, col=col.pred)
      
      # plot(datf$rh~datf$ts, pch=20, cex=0.1, col=col.obs, xlim=c(270-273.16,320-273.16), xlab = "Temperature", ylab="Rel Humidity")
      # points(datf$rh~datf$ts, pch=20, cex=datf$baclass/4, col=col.obs)
      # plot(datf$rh~datf$ts, pch=20, cex=0.1, col=col.pred, xlim=c(270-273.16,320-273.16), xlab = "Temperature", ylab="Rel Humidity")
      # points(datf$rh~datf$ts, pch=20, cex=datf$baclass_pred/4, col=col.pred)
      
      plot(datf$rh~datf$ts, pch=20, cex=0.1, col=col.obs, xlim=c(250,320), xlab = "Temperature", ylab="Rel Humidity")
      points(datf$rh~datf$ts, pch=20, cex=datf$baclass/4, col=col.obs)
      plot(datf$rh~datf$ts, pch=20, cex=0.1, col=col.pred, xlim=c(250,320), xlab = "Temperature", ylab="Rel Humidity")
      points(datf$rh~datf$ts, pch=20, cex=datf$baclass_pred/4, col=col.pred)
      
      
      plot(datf$ts~datf$wsp, pch=20, cex=0.1, col=col.obs, xlab = "Wind speed", ylab="Temperature")
      points(datf$ts~datf$wsp, pch=20, cex=datf$baclass/4, col=col.obs)
      plot(datf$ts~datf$wsp, pch=20, cex=0.1, col=col.pred, xlab = "Wind speed", ylab="Temperature")
      points(datf$ts~datf$wsp, pch=20, cex=datf$baclass_pred/4, col=col.pred)
    
      plot(datf$agri_frac~datf$logpop, pch=20, cex=0.1, col=col.obs, xlab = "Pop density", ylab="Cropland Frac")
      points(datf$agri_frac~datf$logpop, pch=20, cex=datf$baclass/4, col=col.obs)
      plot(datf$agri_frac~datf$logpop, pch=20, cex=0.1, col=col.pred, xlab = "Pop density", ylab="Cropland Frac")
      points(datf$agri_frac~datf$logpop, pch=20, cex=datf$baclass_pred/4, col=col.pred)
    
      # plot(datf$agri_frac~datf$forest_frac, pch=20, cex=0.1, col=col.obs, xlab = "Pop dens", ylab="Agri Frac")
      # points(datf$agri_frac~datf$forest_frac, pch=20, cex=datf$baclass/4, col=col.obs)
      # plot(datf$agri_frac~datf$forest_frac, pch=20, cex=0.1, col=col.pred, xlab = "Pop dens", ylab="Agri Frac")
      # points(datf$agri_frac~datf$forest_frac, pch=20, cex=datf$baclass_pred/3, col=col.pred)
    
      # plot(datf$forest_frac~datf$logpop, pch=20, cex=0.1, col=col.obs, xlab = "Pop dens", ylab="Forest Frac")
      # points(datf$forest_frac~datf$logpop, pch=20, cex=datf$baclass/4, col=col.obs)
      # plot(datf$forest_frac~datf$logpop, pch=20, cex=0.1, col=col.pred, xlab = "Pop dens", ylab="Forest Frac")
      # points(datf$forest_frac~datf$logpop, pch=20, cex=datf$baclass_pred/3, col=col.pred)
      # 
      dev.off()
    }
    
    setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix,"/figures" ))
    
    plot.niche(datf, "ALL")  # MIXED
  
  
  
  }
}

png(filename = paste0("niche_scale.png"), width = 512*3, height = 790*3, res = 300)

par(mfrow = c(4,2), mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)

plot(x=1:11, y=rep(3,11), xlim=c(0,12), ylim=c(1,7), pch=20, cex=(1:11/4), col=colorRampPalette(colors = c("mediumspringgreen","red"))(11)[1:11])
text(x = 1:11, y=2.2)
text("O Fire Class  ", x=6, y=1.5)

points(x=1:11, y=rep(6,11), pch=20, cex=(1:11/4), col=colorRampPalette(colors = c("mediumspringgreen","red"))(11)[1:11])
text(x = 1:11, y=5.2)
text("P Fire Class  ", x=6, y=4.5)

dev.off()

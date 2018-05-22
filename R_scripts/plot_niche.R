# rm(list = ls())
# source(paste0(fire_dir,"/R_scripts/utils.R"))
#### PREDICTED FIRES - CALIBRATION ####

for (model_name in c("full")){
  for (iter in 8:8){
    
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
    system("mkdir figures")

    # datf_train = read.fireData(dataset = "train", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
    # datf_eval = read.fireData(dataset = "eval", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
    # datf_test = read.fireData(dataset = "test", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
    
    # datf = rbind(datf_eval, datf_test, datf_train)
    
    datf = read.fireData(dataset = dataset, dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
    
   
    plot.niche = function(datf, name="", max.baclass=11){
      # png(filename = paste0("niche(",model,"_",name,").png"), width = 400*3, height = 500*3, res = 300)
      png(filename = paste0("niche(",model,"_",name,").png"), width = 512*3, height = 790*3, res = 300)
      
      par(mfrow = c(4,2), mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
      
      plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlab = "Fuel mass", ylab="Fuel Moisture")
      points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
      plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlab = "Fuel mass", ylab="Fuel Moisture")
      points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass_pred/3, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))
      
      # plot(datf$rh~datf$ts, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlim=c(270-273.16,320-273.16), xlab = "Temperature", ylab="Rel Humidity")
      # points(datf$rh~datf$ts, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
      # plot(datf$rh~datf$ts, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlim=c(270-273.16,320-273.16), xlab = "Temperature", ylab="Rel Humidity")
      # points(datf$rh~datf$ts, pch=20, cex=datf$baclass_pred/4, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))
      
      plot(datf$rh~datf$ts, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlim=c(250,320), xlab = "Temperature", ylab="Rel Humidity")
      points(datf$rh~datf$ts, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
      plot(datf$rh~datf$ts, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlim=c(250,320), xlab = "Temperature", ylab="Rel Humidity")
      points(datf$rh~datf$ts, pch=20, cex=datf$baclass_pred/3, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))
      
      
      plot(datf$ts~datf$wsp, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlab = "Wind speed", ylab="Temperature")
      points(datf$ts~datf$wsp, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
      plot(datf$ts~datf$wsp, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlab = "Wind speed", ylab="Temperature")
      points(datf$ts~datf$wsp, pch=20, cex=datf$baclass_pred/3, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))

      plot(datf$agri_frac~datf$logpop, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlab = "Pop density", ylab="Cropland Frac")
      points(datf$agri_frac~datf$logpop, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
      plot(datf$agri_frac~datf$logpop, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlab = "Pop density", ylab="Cropland Frac")
      points(datf$agri_frac~datf$logpop, pch=20, cex=datf$baclass_pred/3, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))

      # plot(datf$agri_frac~datf$forest_frac, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlab = "Pop dens", ylab="Agri Frac")
      # points(datf$agri_frac~datf$forest_frac, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
      # plot(datf$agri_frac~datf$forest_frac, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlab = "Pop dens", ylab="Agri Frac")
      # points(datf$agri_frac~datf$forest_frac, pch=20, cex=datf$baclass_pred/3, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))

      # plot(datf$forest_frac~datf$logpop, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlab = "Pop dens", ylab="Forest Frac")
      # points(datf$forest_frac~datf$logpop, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
      # plot(datf$forest_frac~datf$logpop, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlab = "Pop dens", ylab="Forest Frac")
      # points(datf$forest_frac~datf$logpop, pch=20, cex=datf$baclass_pred/3, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))
      # 
      dev.off()
    }

    setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix,"/figures" ))
    
    plot.niche(datf, "ALL")  # MIXED
    

    
  }
}
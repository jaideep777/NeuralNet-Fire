# rm(list = ls())
# source(paste0(fire_dir,"/R_scripts/utils.R"))
#### PREDICTED FIRES - CALIBRATION ####

sim_name           <- "ssaplus"
suffix = ""
if (sim_name != "") suffix = paste0(suffix,"_",sim_name)
output_dir = paste0("output",suffix)

dataset = "eval"
system("mkdir figures")


# datf = read.csv(file = paste0("/home/jaideep/codes/FIRE_CODES/fire_tensorflow/",dataset,"_forest_.csv"))
# 
# daty = read.delim(paste0("/home/jaideep/codes/FIRE_CODES/fire_tensorflow/y_predic_ba_",dataset,".txt"), header=F, sep=" ")
# # nfires_classes = c(0,1,sqrt(fire_classes[2:length(fire_classes)]* c(fire_classes[3:length(fire_classes)])))
# # nfires_pred = apply(X=daty, MARGIN=1, FUN=function(x){sum(nfires_classes*x)})
# ba_classes = c(0,2^(seq(log2(2^0), log2(2^10), length.out=11)))/1024
# ba_classes_mids = c(0, 0.5/1024, sqrt(ba_classes[3:length(ba_classes)-1]*ba_classes[3:length(ba_classes)]))
# datf$ba.pred = apply(X=daty, MARGIN=1, FUN=function(x){sum(ba_classes_mids*x)})
# datf$baclass_pred = sapply(datf$ba.pred,FUN = function(x){length(which(x>ba_classes))})
# 

datf_train = read.fireData(dataset = "train", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
datf_eval = read.fireData(dataset = "eval", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
datf_test = read.fireData(dataset = "test", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))

# datf = rbind(datf_eval, datf_test, datf_train)

datf = read.fireData(dataset = dataset, dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))

# library(ncdf4)
# library(chron)
# dat = NcCreateOneShot("/home/jaideep/codes/supporting_codes_fire/codes_forest_maps/ftmap_modis/dft_MODIS11lev_agri-bar_lt0.5_0.5deg.nc", var_name = "ft", glimits = c(60.25,99.75,5.25,49.75))
# 
# dat_ids_lat = mapply(datf$lat, FUN=function(x){which(dat$lats == x)})
# dat_ids_lon = mapply(datf$lon, FUN=function(x){which(dat$lons == x)})
# ft = mapply(dat_ids_lon, dat_ids_lat, FUN = function(x,y){dat$data[x,y]})
# 
# plot(x=datf$lon, y=datf$lat, col=heat.colors(12)[ft+1], pch=".", cex=5)
# image(x=dat$lon, y=dat$lat, z=dat$data+1, col=heat.colors(12))
# 
# datf$dft = ft

#### calibration ####

# setwd(paste0("/home/jaideep/codes/FIRE_CODES/figures/",dataset))

plot_calib = function(datf, name, min=1e-3, max=1e-1){
  insuff_data = which(table(datf$baclass_pred)<5)
  for (i in 1:length(insuff_data)){
    datf$baclass_pred[datf$baclass_pred == insuff_data[i]] = NA
  }
  datf = datf[complete.cases(datf),]
  
  f = function(x){
    # log(0.001+x)
    x
  }
  
#  par(mfrow = c(1,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
  obs_ba.predc = tapply(X = datf$ba, INDEX = datf$baclass_pred, FUN=mean)
  pred_ba.predc = tapply(X = datf$ba.pred, INDEX = datf$baclass_pred, FUN=mean)
  plot(obs_ba.predc~pred_ba.predc, log="xy", xlab = "Classwise mean\npredicted BA", ylab = "Classwise mean\nobserved BA", xlim=c(min,max), ylim=c(min,max), cex=1.5, lwd=2)
  abline(0,1,col="red", lwd=2)
  
  #a = summary(lm(obs_ba.predc~pred_ba.predc))
  #mtext(text = sprintf("r = %.2f", a$adj.r.squared), cex=1.5, side=3, adj = 0.1, padj = 2)
  nmse = 1-sum(log(obs_ba.predc[-1])-log(pred_ba.predc[-1]))^2/var(log(obs_ba.predc[-1]))/(length(obs_ba.predc[-1])-1)
  r = cor(obs_ba.predc, pred_ba.predc)
  mtext(text = sprintf("E = %.2f", nmse), cex=1., side=3, adj = 0.1, padj = 2, col="blue")
  mtext(text = sprintf("r = %.2f", r), cex=1., side=3, adj = 0.1, padj = 4, col="blue")
  
  mtext(col="blue",text = paste(name, "(n = ", nrow(datf),", obs = ",sprintf("%.2f",sum(datf$ba)*55.5e3^2*1e-10), " Mha, pred = ",sprintf("%.2f",sum(datf$ba.pred)*55.5e3^2*1e-10)," Mha)"), cex=1.1, side=3, adj = -0., padj = -1.5)  
  
  plot(f(datf$ba)~f(datf$ba.pred), pch=20, cex=0.2, xlab = "Predicted BA", ylab = "Observed BA", xlim=c(0,0.02), ylim=c(0,0.02))
  abline(lm(f(datf$ba)~f(datf$ba.pred)), lwd=3)
  # abline(lm((datf$ba)~f(datf$ba.pred)), col="grey", lwd=3) 
  abline(0,1, col="red", lwd=2)
  
  b = summary(lm(datf$ba~datf$ba.pred))
  nmse_act = 1-sum(f(datf$ba)-f(datf$ba.pred))^2/var(f(datf$ba))/(length(datf$ba)-1)
  r_act = cor(datf$ba, datf$ba.pred)
  # mtext(text = sprintf("r = %.2f", b$adj.r.squared), cex=1.5, side=3, adj = 0.1, padj = 2)
  mtext(text = sprintf("E = %.2f", nmse_act), cex=1., side=3, adj = 0.1, padj = 2, col="blue")
  mtext(text = sprintf("r = %.2f", r_act), cex=1., side=3, adj = 0.1, padj = 4, col="blue")
  # mtext(text = sprintf("Tot Obs  = %.2f Mha", sum(datf$ba)*27.75e3^2*1e-10), cex=1., side=3, adj = 0.1, padj = 4)
  # mtext(text = sprintf("Tot Pred = %.2f Mha", sum(datf$ba.pred)*27.75e3^2*1e-10), cex=1., side=3, adj = 0.1, padj = 5.5)

}

# plot(jitter(datf$baclass)~jitter(datf$baclass_pred), pch=20, cex=1, col=rgb(0,0,0,alpha = 0.1))
# 
# require(reshape2)
# datp = data.frame(obs=datf$ba, pred=f(datf$ba.pred), class = datf$baclass_pred)
# datp2=(melt(datp, id.vars = "class", value.name = "ba", variable.name = "set"))
# boxplot((datp2$ba)~datp2$set+datp2$class, col=c("red","blue"), varwidth=F)
# # require(beanplot)
# # beanplot(log10(datp$pred+1)~datp$class, col=c("red","blue"), what = c(1,1,1,0) )
# tapply(datp2$ba, INDEX = list(datp2$set, datp2$class), FUN = mean)
# tapply(datp2$ba, INDEX = datp2$set, FUN = sum)

# datp = data.frame(obs=datf$ba, pred=f(datf$ba.pred), class = datf$baclass)
# datp2=(melt(datp, id.vars = "class", value.name = "ba", variable.name = "set"))
# boxplot(log10(datp2$ba+1)~datp2$set+datp2$class, col=c("red","blue"), varwidth=F )
# # require(beanplot)
# # beanplot(log10(datp$pred+1)~datp$class, col=c("red","blue"), what = c(1,1,1,0) )
# tapply(datp2$ba, INDEX = list(datp2$set, datp2$class), FUN = mean)
# tapply(datp2$ba, INDEX = datp2$set, FUN = sum)


# plot(x=jitter(log(datp$obs+1)), y=jitter(log(datp$pred+1)), pch=".", cex=2)
# abline(lm(log(datp$obs+1)~log(datp$pred+1)-1),col="green4")
# abline(0,1,col="red")
# abline(lm(log(datp$pred[datp$obs>20]+1)~log(datp$obs[datp$obs>20]+1)-1))
# abline(lm(log(datp$pred[datp$obs<20]+1)~log(datp$obs[datp$obs<20]+1)-1))


#### Pred and obs phase plots with classes ####

# png(filename = "pred_classes.png", width = 400*3, height = 500*3, res = 300)
# 
# par(mfrow = c(3,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
# 
# plot(datf$baclass_pred~datf$lmois, col=rgb(1-datf$dxl/max(datf$dxl), datf$dxl/max(datf$dxl), 0), pch=20)
# plot(datf$baclass~datf$lmois, col=rgb(1-datf$dxl/max(datf$dxl), datf$dxl/max(datf$dxl), 0), pch=20)
# 
# plot(datf$baclass_pred~datf$dxl, col=rgb(1-datf$lmois, 0, datf$lmois))
# plot(datf$baclass~datf$dxl, col=rgb(1-datf$lmois, 0, datf$lmois))
# 
# plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max(datf$baclass_pred), 1-datf$baclass_pred/max(datf$baclass_pred), 0))
# points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass_pred/4, col=rgb(datf$baclass_pred/max(datf$baclass_pred), 1-datf$baclass_pred/max(datf$baclass_pred), 0))
# # plot(datf$lmois~datf$dxl, pch=20, cex=1, col=rgb(datf$baclass_pred>0, datf$baclass_pred==0, 0,alpha = 0.3))
# # par(mfrow = c(2,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
# plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=rgb(datf$baclass/max(datf$baclass), 1-datf$baclass/max(datf$baclass), 0))
# points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max(datf$baclass), 1-datf$baclass/max(datf$baclass), 0))
# # plot(datf$lmois~datf$dxl, pch=20, cex=1, col=rgb(datf$baclass>0, datf$baclass==0, 0,alpha = 0.3))
# 
# dev.off()

#### Pred and obs phase plots with BA ####

plot.niche = function(datf, name="", max.baclass=11){
  png(filename = paste0("niche(",model,"_",name,").png"), width = 400*3, height = 500*3, res = 300)
  
  par(mfrow = c(3,2), mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
  
  plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlab = "Fuel mass", ylab="Fuel Moisture")
  points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
  plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlab = "Fuel mass", ylab="Fuel Moisture")
  points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass_pred/4, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))
  
  # plot(datf$rh~datf$ts, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlim=c(270-273.16,320-273.16), xlab = "Temperature", ylab="Rel Humidity")
  # points(datf$rh~datf$ts, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
  # plot(datf$rh~datf$ts, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlim=c(270-273.16,320-273.16), xlab = "Temperature", ylab="Rel Humidity")
  # points(datf$rh~datf$ts, pch=20, cex=datf$baclass_pred/4, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))

  plot(datf$rh~datf$ts, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlim=c(270,320), xlab = "Temperature", ylab="Rel Humidity")
  points(datf$rh~datf$ts, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
  plot(datf$rh~datf$ts, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlim=c(270,320), xlab = "Temperature", ylab="Rel Humidity")
  points(datf$rh~datf$ts, pch=20, cex=datf$baclass_pred/4, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))
  
    
  plot(datf$ts~datf$wsp, pch=20, cex=0.1, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0), xlab = "Wind speed", ylab="Temperature")
  points(datf$ts~datf$wsp, pch=20, cex=datf$baclass/4, col=rgb(datf$baclass/max.baclass, 1-datf$baclass/max.baclass, 0))
  plot(datf$ts~datf$wsp, pch=20, cex=0.1, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0), xlab = "Wind speed", ylab="Temperature")
  points(datf$ts~datf$wsp, pch=20, cex=datf$baclass_pred/4, col=rgb(datf$baclass_pred/max.baclass, 1-datf$baclass_pred/max.baclass, 0))
  
  dev.off()
}
# #### Pred and obs vs vars ####
# 
# datf = datf[order(datf$date), ]
# ts_obs = tapply(X = datf$ba, INDEX = datf$date, FUN = sum)
# ts_pred = tapply(X = datf$ba.pred, INDEX = datf$date, FUN = sum)
# 
# png(filename = "pred_obs_vs_vars.png", width = 340*3, height = 600*3, res = 300)
# 
# par(mfrow = c(4,1), mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
# 
# plot(ts_obs~as.Date(names(ts_obs)), type="o", col="grey", xlab="Time", ylab="Burned area")
# points(ts_obs~as.Date(names(ts_obs)), col="grey4")
# points(ts_pred~as.Date(names(ts_pred)), col="blue", type="l", lwd=2)
# 
# 
# dxl_cuts = cut(datf$dxl, breaks = seq(0,300, by=10))
# plot(x= head(filter(seq(0,300, by=10), filter = c(0.5,0.5)), -1), y=tapply(datf$ba, INDEX = dxl_cuts, FUN = mean), col="green2", lwd=2, ylab="Burned area", xlab="Fuel mass")
# points(x= head(filter(seq(0,300, by=10), filter = c(0.5,0.5)), -1), y=tapply(datf$ba.pred, INDEX = cut(datf$dxl, breaks = seq(0,300, by=10)), FUN = mean), type="l", lwd=2, col="green4")
# 
# plot(tapply(datf$ba, INDEX = cut(datf$lmois, breaks = seq(0,1, by=0.05)), FUN = mean), col="cyan3", lwd=2, ylab="Burned area", xlab="Fuel moisture")
# points(tapply(datf$ba.pred, INDEX = cut(datf$lmois, breaks = seq(0,1, by=0.05)), FUN = mean), type="l", lwd=2, col="blue")
# 
# plot(tapply(datf$ba, INDEX = cut(datf$ts, breaks = seq(250,320, by=5)), FUN = mean), col="orange2", lwd=2, ylab="Burned area", xlab="Temperature")
# points(tapply(datf$ba.pred, INDEX = cut(datf$ts, breaks = seq(250,320, by=5)), FUN = mean), type="l", lwd=2, col="red")
# 
# dev.off()

# plot(tapply(datf$ba, INDEX = cut(datf$rh, breaks = seq(0,110, by=5)), FUN = mean), col="orange2", lwd=2, ylab="Burned area", xlab="Temperature")
# points(tapply(datf$ba.pred, INDEX = cut(datf$rh, breaks = seq(0,110, by=5)), FUN = mean), type="l", lwd=2, col="red")
# 
# plot(tapply(datf$ba, INDEX = cut(datf$wsp, breaks = seq(0,8, by=.2)), FUN = mean), col="orange2", lwd=2, ylab="Burned area", xlab="Temperature")
# points(tapply(datf$ba.pred, INDEX = cut(datf$wsp, breaks = seq(0,8, by=.2)), FUN = mean), type="l", lwd=2, col="red")
# 
# plot(tapply(datf$ba, INDEX = cut(datf$forest_frac, breaks = seq(0,1, by=.01)), FUN = mean), col="orange2", lwd=2, ylab="Burned area", xlab="Temperature")
# points(tapply(datf$ba.pred, INDEX = cut(datf$forest_frac, breaks = seq(0,1, by=.01)), FUN = mean), type="l", lwd=2, col="red")
# 
# plot(tapply(datf$ba, INDEX = cut(datf$agri_frac, breaks = seq(0,1, by=.01)), FUN = mean), col="orange2", lwd=2, ylab="Burned area", xlab="Temperature")
# points(tapply(datf$ba.pred, INDEX = cut(datf$agri_frac, breaks = seq(0,1, by=.01)), FUN = mean), type="l", lwd=2, col="red")
# 
setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix,"/figures" ))

iX   = 0
iAGR = 1
iBLE = 2
iNLE = 3
iMD  = 4
iDD  = 5
iGR  = 6
iSC  = 7
iMX  = 8



# plot_calib(datf, "ALL")  # ALL (train, test, eval)

pfts_ssaplus = c(0, 1, 6, 10, 2, 7, 9, 11)
pftnames_ssaplus = c("Barren", "NLE", "SCX", "AGR", "BLE", "MD", "GR", "MX")

# model = "full"
# for (i in 1:length(pfts_ssaplus)){ 
#   plot.niche(datf[datf$dft==pfts_ssaplus[i],], pftnames_ssaplus[i])  # X
# }
plot.niche(datf, "ALL")  # MIXED

png(filename = "calib_all.png", width = 300*6, height = 500*6, res=300)
par(mfrow = c(4,2), mar=c(5,7,4,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5, mgp=c(4,1,0))
plot_calib(datf, "ALL")  # MIXED
dev.off()

png(filename = "PFTwise_1.png", width = 300*6, height = 500*6, res=300)
par(mfrow = c(4,2), mar=c(5,7,4,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5, mgp=c(4,1,0))
for (i in 1:4){
  plot_calib(datf[datf$dft==pfts_ssaplus[i],], pftnames_ssaplus[i])  # X
}
dev.off()

png(filename = "PFTwise_2.png", width = 300*6, height = 500*6, res=300)
par(mfrow = c(4,2), mar=c(5,7,4,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5, mgp=c(4,1,0))
for (i in 5:8){
  plot_calib(datf[datf$dft==pfts_ssaplus[i],], pftnames_ssaplus[i])  # X
}
dev.off()




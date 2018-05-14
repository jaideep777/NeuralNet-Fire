
plot.relations = function(datf.fn, dataset="Training"){
  dxl_cuts = cut(datf.fn$dxl, breaks = seq(0,300, by=10))
  plot(x= mids(seq(0,300, by=10)), y=tapply(datf.fn$ba, INDEX = dxl_cuts, FUN = mean), col="green2", lwd=2, ylab="Burned area", xlab="Fuel mass",ylim = c(0,0.020))
  points(x= mids(seq(0,300, by=10)), y=tapply(datf.fn$ba.pred, INDEX = dxl_cuts, FUN = mean), type="l", lwd=2, col="green4")
  #add_label(0.00, 0.1, label ="sum", cex=1.5, pos=4.2)
  #add_label(0.00, 0.3, label =sum(datf.fn$ba), cex=1.5, pos=4.2)
  r2 = summary(lm(tapply(datf.fn$ba, INDEX = dxl_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = dxl_cuts, FUN = mean)))$adj.r.squared
  add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  mtext(side = 3, text = dataset, line=0.5)
  
  lmois_cuts = cut(datf.fn$lmois, breaks = seq(0,1, by=0.05))
  plot(x = mids(seq(0,1, by=0.05)), y=tapply(datf.fn$ba, INDEX = lmois_cuts, FUN = mean),col="cyan3", lwd=2, ylab="Burned area", xlab="Fuel moisture",ylim = c(0,0.020))
  points(x = mids(seq(0,1, by=0.05)), y=tapply(datf.fn$ba.pred, INDEX = lmois_cuts, FUN = mean), type="l", lwd=2, col="blue")
  #add_label(0.00, 0.1, label ="Atr and At", cex=1.5, pos=4.2)
  #add_label(0.00, 0.3, label =ATr, cex=1.5, pos=4.2)
  #add_label(0.00, 0.5, label =AT, cex=1.5, pos=4.2)
  r2 = summary(lm(tapply(datf.fn$ba, INDEX = lmois_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = lmois_cuts, FUN = mean)))$adj.r.squared
  add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  
  ts_cuts = cut(datf.fn$ts, breaks = seq(250,320, by=5))
  plot(x=mids(seq(250,320, by=5)), y=tapply(datf.fn$ba, INDEX = ts_cuts, FUN = mean), col="orange2", lwd=2, ylab="Burned area", xlab="Temperature",ylim = c(0,0.020))
  points(x=mids(seq(250,320, by=5)), y=tapply(datf.fn$ba.pred, INDEX = ts_cuts, FUN = mean), type="l", lwd=2, col="red")
  r2 = summary(lm(tapply(datf.fn$ba, INDEX = ts_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = ts_cuts, FUN = mean)))$adj.r.squared
  add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  
  rh_cuts = cut(datf.fn$rh, breaks = seq(0,110, by=5))
  plot(x=mids(seq(0,110, by=5)), y=tapply(datf.fn$ba, INDEX = rh_cuts, FUN = mean), col="magenta", lwd=2, ylab="Burned area", xlab="Relative humidity",ylim = c(0,0.020))
  points(x=mids(seq(0,110, by=5)), y=tapply(datf.fn$ba.pred, INDEX = rh_cuts, FUN = mean), type="l", lwd=2, col="magenta4")
  r2 = summary(lm(tapply(datf.fn$ba, INDEX = rh_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = rh_cuts, FUN = mean)))$adj.r.squared
  add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  
  wsp_cuts = cut(datf.fn$wsp, breaks = seq(0,8, by=.2))
  plot(x= mids(seq(0,8, by=.2)), y=tapply(datf.fn$ba, INDEX = wsp_cuts, FUN = mean), col=rgb(.6,.6,.6), lwd=2, ylab="Burned area", xlab="Windspeed",ylim = c(0,0.020))
  points(x= mids(seq(0,8, by=.2)), y=tapply(datf.fn$ba.pred, INDEX = wsp_cuts, FUN = mean), type="l", lwd=2, col=rgb(.3,.3,.3))
  r2 = summary(lm(tapply(datf.fn$ba, INDEX = wsp_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = wsp_cuts, FUN = mean)))$adj.r.squared
  add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)

  pop_cuts = cut(datf.fn$logpop, breaks = seq(0,8, by=.2))
  plot(x= mids(seq(0,8, by=.2)), y=tapply(datf.fn$ba, INDEX = pop_cuts, FUN = mean), col="goldenrod", lwd=2, ylab="Burned area", xlab="Log pop density",ylim = c(0,0.040))
  points(x= mids(seq(0,8, by=.2)), y=tapply(datf.fn$ba.pred, INDEX = pop_cuts, FUN = mean), type="l", lwd=2, col="goldenrod4")
  r2 = summary(lm(tapply(datf.fn$ba, INDEX = pop_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = pop_cuts, FUN = mean)))$adj.r.squared
  add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  
}



model = "full"

datf_train = read.fireData_gfed(dataset = "train", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
datf_eval = read.fireData_gfed(dataset = "eval", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
datf_test = read.fireData_gfed(dataset = "test", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))

dat.ag_train = datf_train
dat.ag_test = datf_test
dat.ag_eval = datf_eval

dat.ag_train = dat.ag_train[order(dat.ag_train$date), ]
ts_obs_train = tapply(X = dat.ag_train$ba, INDEX = dat.ag_train$date, FUN = sum)
ts_pred_train = tapply(X = dat.ag_train$ba.pred, INDEX = dat.ag_train$date, FUN = sum)

dat.ag_test = dat.ag_test[order(dat.ag_test$date), ]
ts_obs_test = tapply(X = dat.ag_test$ba, INDEX = dat.ag_test$date, FUN = sum)
ts_pred_test = tapply(X = dat.ag_test$ba.pred, INDEX = dat.ag_test$date, FUN = sum)

dat.ag_eval = dat.ag_eval[order(dat.ag_eval$date), ]
ts_obs_eval = tapply(X = dat.ag_eval$ba, INDEX = dat.ag_eval$date, FUN = sum)
ts_pred_eval = tapply(X = dat.ag_eval$ba.pred, INDEX = dat.ag_eval$date, FUN = sum)

setwd(paste0(fire_dir, "/figures",suffix))
png(filename = paste0("pred_obs_vars(",model,")_test.png"), width = 440*3, height = 1050*3, res = 300)

layout(cbind(c(1,2,3,4,5,6,7),c(1,8,9,10,11,12,13)))
par(mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
train_dates = as.Date(names(ts_obs_train))
test_dates = as.Date(names(ts_obs_test))
ts_pred_train[which(diff(train_dates) > 30)+1] = NA # just to break the line at break in dates
plot(y=ts_obs_train+ts_obs_eval, x=train_dates, col="cyan3", xlab="Time", ylab="Burned area",ylim=c(0,8))
# points(ts_obs_train~as.Date(names(ts_obs_train)), col="grey", type="l")
# points(ts_obs_test~as.Date(names(ts_obs_test)), col="grey",type="l")
# add_label(0.00, 0.1, label ="N and L"  ,cex=1.7, pos=4.2)
# add_label(0.00, 0.3, label = N ,cex=1.7, pos=4.2)
# add_label(0.00, 0.5, label = L ,cex=1.7, pos=4.2)
points(ts_obs_test~as.Date(names(ts_obs_test)), col="magenta", xlab="Time", ylab="Burned area")
points(y=ts_pred_train+ts_pred_eval, x=as.Date(names(ts_pred_train)), col="blue", type="l", lwd=2)
points(ts_pred_test~as.Date(names(ts_pred_test)), type="l", col="magenta4", lwd=2)

r2 = summary(lm(ts_obs_eval~ts_pred_eval))$adj.r.squared
add_label(0.00, 0.15, label = sprintf("R2 (eval) = %.2f", r2), cex=1.5, pos=4)

plot.relations(dat.ag_train)


# add_label(0.00, 0.3, label =p, cex=1.5, pos=4.2)
# add_label(0.00, 0.5, label =cor, cex=1.5, pos=4.2)
plot.relations(dat.ag_eval, "Evaluation")
# add_label(0.00, 0.1, label ="ce", cex=1.5, pos=4.2)
# add_label(0.00, 0.3, label =CE, cex=1.5, pos=4.2)


dev.off()


# source("/home/jaideep/codes/FIRE_CODES/R_scripts/plot_calibration.R")



# source("/home/jaideep/codes/FIRE_CODES/R_scripts/plot_maps.R")




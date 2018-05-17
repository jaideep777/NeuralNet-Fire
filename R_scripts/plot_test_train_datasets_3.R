# Simulation name ("" or "india" or "ssaplus" etc)
for (model_name in c("full")){
for (iter in 1:10){

model = paste0(model_name, "_", iter)

# Simulation name ("" or "india" or "ssaplus" etc)
sim_name           <- paste0("ssaplus", "/", model)

# Directories to the fire model folder
fire_dir           <- "/home/jaideep/codes/FIRE_CODES" # root directory for fire codes

#### Init ####
suffix = ""
if (sim_name != "") suffix = paste0(suffix,"_",sim_name)
output_dir = paste0("output",suffix)





plot.cut.means = function(obs, pred, var, min, max, col.obs, col.pred, ...){
  brks = seq(min,max, length.out=21)
  cuts = cut(var, breaks = brks, include.lowest = T)
  plot(x= mids(brks), y=tapply(obs, INDEX = cuts, FUN = mean), col=col.obs, lwd=2, ... ,ylim = c(0,0.010))
  points(x= mids(brks), y=tapply(pred, INDEX = cuts, FUN = mean), type="l", lwd=2, col=col.pred)
  #add_label(0.00, 0.1, label ="sum", cex=1.5, pos=4.2)
  #add_label(0.00, 0.3, label =sum(datf.fn$ba), cex=1.5, pos=4.2)
  r2 = summary(lm(tapply(obs, INDEX = cuts, FUN = mean)~tapply(pred, INDEX = cuts, FUN = mean)))$adj.r.squared
  r = cor(tapply(obs, INDEX = cuts, FUN = mean), tapply(pred, INDEX = cuts, FUN = mean), use = "pairwise.complete.obs")
  add_label(0.00, 0.15, label = sprintf("R = %.2f", r), cex=1.5, pos=4)
  tapply(obs, INDEX = cuts, FUN = length)
}

plot.relations = function(datf.fn, dataset="Training"){
  # breaks_dxl = seq(0,300, length.out=20)
  # dxl_cuts = cut(datf.fn$dxl, breaks = breaks_dxl)
  # plot(x= mids(breaks_dxl), y=tapply(datf.fn$ba, INDEX = dxl_cuts, FUN = mean), col="green2", lwd=2, ylab="Burned area", xlab="Fuel mass",ylim = c(0,0.020))
  # points(x= mids(breaks_dxl), y=tapply(datf.fn$ba.pred, INDEX = dxl_cuts, FUN = mean), type="l", lwd=2, col="green4")
  # #add_label(0.00, 0.1, label ="sum", cex=1.5, pos=4.2)
  # #add_label(0.00, 0.3, label =sum(datf.fn$ba), cex=1.5, pos=4.2)
  # r2 = summary(lm(tapply(datf.fn$ba, INDEX = dxl_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = dxl_cuts, FUN = mean)))$adj.r.squared
  # add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  # mtext(side = 3, text = dataset, line=0.5)
  plot.cut.means(obs = datf.fn$ba, pred = datf.fn$ba.pred, var = datf.fn$dxl, min = 0, max = 300, col.obs = "green2", col.pred = "green4", xlab="Fuel mass", ylab="Burned area")
  mtext(side = 3, text = dataset, line=0.5, col="blue")
  
  plot.cut.means(obs = datf.fn$ba, pred = datf.fn$ba.pred, var = datf.fn$lmois, min = 0, max = 1, col.obs = "cyan3", col.pred = "blue", xlab="Fuel moisture", ylab="Burned area")
  # plot.cut.means(obs = datf.fn$ba, pred = datf.fn$ba.pred, var = datf.fn$ts, min = 250-273.16, max = 320-273.16, col.obs = "orange2", col.pred = "red", xlab="Temperature", ylab="Burned area")
  plot.cut.means(obs = datf.fn$ba, pred = datf.fn$ba.pred, var = datf.fn$ts, min = 250, max = 320, col.obs = "orange2", col.pred = "red", xlab="Temperature", ylab="Burned area")
  plot.cut.means(obs = datf.fn$ba, pred = datf.fn$ba.pred, var = datf.fn$rh, min = 0, max = 110, col.obs = "magenta", col.pred = "magenta4", xlab="Rel humidity", ylab="Burned area")
  plot.cut.means(obs = datf.fn$ba, pred = datf.fn$ba.pred, var = datf.fn$wsp, min = 0, max = 8, col.obs = "grey60", col.pred = "grey30", xlab="Wind speed", ylab="Burned area")
  plot.cut.means(obs = datf.fn$ba, pred = datf.fn$ba.pred, var = datf.fn$logpop, min = 0, max = 8, col.obs = "goldenrod", col.pred = "goldenrod4", xlab="Log pop density", ylab="Burned area")
  
  
  # breaks_ts = seq(250,320, length.out=20)
  # ts_cuts = cut(datf.fn$ts, breaks = breaks_ts)
  # plot(x=mids(breaks_ts), y=tapply(datf.fn$ba, INDEX = ts_cuts, FUN = mean), col="orange2", lwd=2, ylab="Burned area", xlab="Temperature",ylim = c(0,0.020))
  # points(x=mids(breaks_ts), y=tapply(datf.fn$ba.pred, INDEX = ts_cuts, FUN = mean), type="l", lwd=2, col="red")
  # r2 = summary(lm(tapply(datf.fn$ba, INDEX = ts_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = ts_cuts, FUN = mean)))$adj.r.squared
  # add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  # 
  # rh_cuts = cut(datf.fn$rh, breaks = seq(0,110, length.out=20))
  # plot(x=mids(seq(0,110, by=5)), y=tapply(datf.fn$ba, INDEX = rh_cuts, FUN = mean), col="magenta", lwd=2, ylab="Burned area", xlab="Relative humidity",ylim = c(0,0.020))
  # points(x=mids(seq(0,110, by=5)), y=tapply(datf.fn$ba.pred, INDEX = rh_cuts, FUN = mean), type="l", lwd=2, col="magenta4")
  # r2 = summary(lm(tapply(datf.fn$ba, INDEX = rh_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = rh_cuts, FUN = mean)))$adj.r.squared
  # add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  # 
  # wsp_cuts = cut(datf.fn$wsp, breaks = seq(0,8, length.out=20))
  # plot(x= mids(seq(0,8, by=.2)), y=tapply(datf.fn$ba, INDEX = wsp_cuts, FUN = mean), col=rgb(.6,.6,.6), lwd=2, ylab="Burned area", xlab="Windspeed",ylim = c(0,0.020))
  # points(x= mids(seq(0,8, by=.2)), y=tapply(datf.fn$ba.pred, INDEX = wsp_cuts, FUN = mean), type="l", lwd=2, col=rgb(.3,.3,.3))
  # r2 = summary(lm(tapply(datf.fn$ba, INDEX = wsp_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = wsp_cuts, FUN = mean)))$adj.r.squared
  # add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  # 
  # pop_cuts = cut(datf.fn$logpop, breaks = seq(0,8,length.out=20))
  # plot(x= mids(seq(0,8, by=.2)), y=tapply(datf.fn$ba, INDEX = pop_cuts, FUN = mean), col="goldenrod", lwd=2, ylab="Burned area", xlab="Log pop density",ylim = c(0,0.040))
  # points(x= mids(seq(0,8, by=.2)), y=tapply(datf.fn$ba.pred, INDEX = pop_cuts, FUN = mean), type="l", lwd=2, col="goldenrod4")
  # r2 = summary(lm(tapply(datf.fn$ba, INDEX = pop_cuts, FUN = mean)~tapply(datf.fn$ba.pred, INDEX = pop_cuts, FUN = mean)))$adj.r.squared
  # add_label(0.00, 0.15, label = sprintf("R2 = %.2f", r2), cex=1.5, pos=4)
  
}



# model = "full"

nn_offset = 0.002

datf_train = read.fireData(dataset = "train", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir), nn_offset = nn_offset)
datf_eval = read.fireData(dataset = "eval", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir), nn_offset = nn_offset)
datf_test = read.fireData(dataset = "test", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir), nn_offset = nn_offset)

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

setwd(paste0(fire_dir, "/fire_aggregateData/",output_dir,"/figures"))
png(filename = paste0("pred_obs_vars(",model,")_nn_off_",nn_offset,".png"), width = 660*3, height = 1050*3, res = 300)

layout(matrix(data=c( 1, 2,3,4,5,6,7,
                      1, 8,9,10,11,12,13,
                      1, 14,15,16,17,18,19) , ncol = 3,byrow = F)
      )
par(mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
train_dates = as.Date(names(ts_obs_train))
test_dates = as.Date(names(ts_obs_test))
ts_pred_train[which(diff(train_dates) > 40)+1] = NA # just to break the line at break in dates
plot(y=ts_obs_train+ts_obs_eval, x=train_dates, col="cyan3", xlab="", ylab="Burned area",ylim=c(0,8))
# points(ts_obs_train~as.Date(names(ts_obs_train)), col="grey", type="l")
# points(ts_obs_test~as.Date(names(ts_obs_test)), col="grey",type="l")
# add_label(0.00, 0.1, label ="N and L"  ,cex=1.7, pos=4.2)
# add_label(0.00, 0.3, label = N ,cex=1.7, pos=4.2)
# add_label(0.00, 0.5, label = L ,cex=1.7, pos=4.2)
points(ts_obs_test~as.Date(names(ts_obs_test)), col="magenta", xlab="Time", ylab="Burned area")
points(y=ts_pred_train+ts_pred_eval, x=as.Date(names(ts_pred_train)), col="blue", type="l", lwd=2)
points(ts_pred_test~as.Date(names(ts_pred_test)), type="l", col="magenta4", lwd=2)

r2 = summary(lm(ts_obs_eval~ts_pred_eval))$adj.r.squared
r = cor(ts_obs_eval, ts_pred_eval)
add_label(0.00, 0.15, label = sprintf("R (eval) = %.2f", r), cex=1.5, pos=4)


plot.relations(dat.ag_train, "Training")


# add_label(0.00, 0.3, label =p, cex=1.5, pos=4.2)
# add_label(0.00, 0.5, label =cor, cex=1.5, pos=4.2)
plot.relations(dat.ag_eval, "Evaluation")
# add_label(0.00, 0.1, label ="ce", cex=1.5, pos=4.2)
# add_label(0.00, 0.3, label =CE, cex=1.5, pos=4.2)

plot.relations(dat.ag_test, "Test")


dev.off()


# source("/home/jaideep/codes/FIRE_CODES/R_scripts/plot_calibration.R")



# source("/home/jaideep/codes/FIRE_CODES/R_scripts/plot_maps.R")
}
}

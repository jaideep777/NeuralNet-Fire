#### SINGLE POINT OUTPUT ####

# plot(dat$npp~as.Date(dat$date), type="l")
dat = read.csv(file = paste0("/home/jaideep/codes/FIRE_CODES/fire_calcFuel/output_",sim_name,"/point_run.txt"), sep="\t",header=T, skip=4)

daty = read.delim("/home/jaideep/codes/FIRE_CODES/fire_tensorflow/y_predic_ba.txt", header=F, sep=" ")
# nfires_classes = c(0,1,sqrt(fire_classes[2:length(fire_classes)]* c(fire_classes[3:length(fire_classes)])))
# nfires_pred = apply(X=daty, MARGIN=1, FUN=function(x){sum(nfires_classes*x)})
ba_classes_mids = c(0, 0.5/1024, sqrt(ba_classes[3:length(ba_classes)-1]*ba_classes[3:length(ba_classes)]))
ba_pred = apply(X=daty, MARGIN=1, FUN=function(x){sum(ba_classes_mids*x)})
baclass_pred = sapply(ba_pred,FUN = function(x){length(which(x>ba_classes))})

datf = read.csv(file = "/home/jaideep/codes/FIRE_CODES/fire_tensorflow/train_forest.csv")

mudu_ids = which(datf$lat == 11.5 & datf$lon == 76.5)
dat_mudu = datf[mudu_ids,]
# dat_mudu$nfires.pred = nfires_pred[mudu_ids]
dat_mudu$ba.pred = ba_pred[mudu_ids]

dat_mudu = dat_mudu[order(dat_mudu$year, dat_mudu$month, dat_mudu$day), ]

setwd("/home/jaideep/codes/FIRE_CODES/figures")

png("sp_out.png", height=600*3, width=500*3, res=300)

dat1 = dat[as.Date(dat$date)>=as.Date("2007-1-1"),]
par(mfrow=c(6,1), mar=c(0.,6,0,1), oma=c(4,1,1,1), cex.lab=1.5, cex.axis=1.5, las=2)
plot(dat1$ndr~as.Date(dat1$date), type="l", col="red", xaxt="n", ylab="Net downward\nradiation")
plot(dat1$npp~as.Date(dat1$date), type="l", col="red", xaxt="n", ylab="Net primary\nproductivity")
# plot(dat1$cld~as.Date(dat1$date), type="l", col="grey", xaxt="n", ylab="Cloud\ncover")
plot(dat1$ts~as.Date(dat1$date), type="l", col="magenta", xaxt="n", ylab="Temperature")
plot(y=dat1$pr,x=as.Date(dat1$date), type="l", col="blue", xaxt="n", ylab="Rainfall")
plot(y=dat1$dxl,x=as.Date(dat1$date), type="l", col="green3", xaxt="n", ylab="Fuel\nbiomass")
plot(y=dat1$lmois,x=as.Date(dat1$date), type="l", col="cyan3", xaxt="n", ylab="Fuel\nmoisture")

f = function(x){
  0.0005*(exp(x/0.002)-1)
}

plot(y=1:length(dat1$date)*NA,x=as.Date(dat1$date), type="l", col="yellow3", ylab="Burned\narea", ylim=c(0*min(dat_mudu$ba, dat_mudu$ba.pred), max(dat_mudu$ba, dat_mudu$ba.pred)))
with(dat_mudu, points(x=as.Date(sprintf("%g-%g-%g",year,month,day)), y=ba, col="yellow3", pch=20, cex=2))
with(dat_mudu, points(x=as.Date(sprintf("%g-%g-%g",year,month,day)), y=f(ba.pred), col="orange", cex=2, lwd=2) )

# with(dat_mudu, plot(x=as.Date(sprintf("%g-%g-15",year,month)), y=ffev, type="o", col="red") )
# with(dat_mudu, points(x=as.Date(sprintf("%g-%g-15",year,month)), y=nfires.pred, type="o", col="orange") )

dev.off()

dat_mudu = dat_mudu[order(dat_mudu$ba.pred), ]


plot(dat_mudu$ba~dat_mudu$ba.pred)
points(f(dat_mudu$ba.pred)~dat_mudu$ba.pred, type="l")


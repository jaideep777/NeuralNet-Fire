
#### AGGREGATED DATA ####

datm = read.delim("/home/jaideep/codes/FIRE_CODES/fire_aggregateData/output/train_data.txt", header=T)
datm[datm==9.9e20] = NA
datm = datm[,-length(datm)]
# datm$ba = datm$ba / 27.75e3^2
# fire_classes = c(0,1,4,16,64,256,1024)
ba_classes = c(0,2^(seq(log2(2^0), log2(2^10), length.out=11)))/1024
# datm$fireclass = sapply(datm$ffev,FUN = function(x){length(which(x>fire_classes))})
datm$baclass = sapply(datm$ba,FUN = function(x){length(which(x>ba_classes))})

par(mfrow = c(1,1), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
dat_bad = datm[!complete.cases(datm),]
with(dat_bad, plot((lat)~(lon), pch=".", xlim=c(66.5,100.5), ylim=c(6.5,38.5)))
dat_good = datm[complete.cases(datm),]
datf = dat_good[dat_good$forest_frac>0.3,]
# datf = datf[datf$baclass>0,]
with(datf, points(jitter(lat)~jitter(lon), pch=".", col="green2", cex=3))


par(mfrow = c(2,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
plot(datf$fireclass~datf$lmois, col=rgb(1-datf$dxl/max(datf$dxl), datf$dxl/max(datf$dxl), 0))
plot(datf$fireclass~datf$dxl, col=rgb(1-datf$lmois, 0, datf$lmois))
plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=rgb(datf$fireclass/max(datf$fireclass), 1-datf$fireclass/max(datf$fireclass), 0))
points(datf$lmois~datf$dxl, pch=20, cex=datf$fireclass/5, col=rgb(datf$fireclass/max(datf$fireclass), 1-datf$fireclass/max(datf$fireclass), 0))
plot(datf$lmois~datf$dxl, pch=20, cex=1, col=rgb(datf$ffev>0, datf$ffev==0, 0,alpha = 0.2))

par(mfrow = c(2,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
plot(datf$baclass~datf$lmois, col=rgb(1-datf$dxl/max(datf$dxl), datf$dxl/max(datf$dxl), 0), pch=20)
plot(datf$baclass~datf$dxl, col=rgb(1-datf$lmois, 0, datf$lmois))
plot(datf$lmois~datf$dxl, pch=20, cex=0.1, col=rgb(datf$baclass/max(datf$baclass), 1-datf$baclass/max(datf$baclass), 0))
points(datf$lmois~datf$dxl, pch=20, cex=datf$baclass/3, col=rgb(datf$baclass/max(datf$baclass), 1-datf$baclass/max(datf$baclass), 0))
plot(datf$lmois~datf$dxl, pch=20, cex=1, col=rgb(datf$baclass>0, datf$baclass==0, 0,alpha = 0.3))

par(mfrow = c(1,2), mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
log_events_pos = log10(as.numeric(table(datf$ffev[datf$ffev>0])))
log_nfires_pos = log10(as.numeric(names(table(datf$ffev[datf$ffev>0]))))
plot(log_events_pos~log_nfires_pos, xlab = "log10(Number of fires)", ylab="log10(Frequency)")
abline(2.5,-1.1, lwd=2)
mtext(text = "A", side = 3, at = 2.8, cex=2, line = -2)

log_events = log10(as.numeric(table(datf$fireclass)))
classes = as.numeric(names(table(datf$fireclass)))
barplot(height = log_events,names.arg = classes,ylim = c(0,4), xlab="Fire class", ylab="log10(Frequency)")
mtext(text = "B", side = 3, at = 8, cex=2, line = -2)

png("ba_dist.png", width = 640*3, height = 330*3, res = 300)

par(mfrow = c(1,2), mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
log_events_pos = log10(as.numeric(table(datf$ba[datf$ba>0])))
log_ba_pos = log10(as.numeric(names(table(datf$ba[datf$ba>0]))))
plot(log_events_pos~log_ba_pos, xlab = "log10(Burned area)", ylab="log10(Frequency)")
abline(-1,-1.9/3, lwd=2)
mtext(text = "A", side = 3, at = 2.8, cex=2, line = -2)

log_events = log10(as.numeric(table(datf$baclass)))
classes = as.numeric(names(table(datf$baclass)))
barplot(height = log_events,names.arg = classes,ylim = c(0,4), xlab="Fire class", ylab="log10(Frequency)")
mtext(text = "B", side = 3, at = 8, cex=2, line = -2)

dev.off()






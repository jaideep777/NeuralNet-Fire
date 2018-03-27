dat.ag = dat_test

dat.ag = dat.ag[order(dat.ag$date), ]
ts_obs = tapply(X = dat.ag$ba, INDEX = dat.ag$date, FUN = sum)
# ts_pred = tapply(X = dat.ag$ba.pred, INDEX = dat.ag$date, FUN = sum)

# png(filename = "pred_obs_vs_vars.png", width = 340*3, height = 600*3, res = 300)

layout(cbind(c(1,2,3,4),c(1,5,6,7)))
par(mar=c(4,5,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)

plot(ts_obs~as.Date(names(ts_obs)), type="o", col="grey", xlab="Time", ylab="Burned area")
points(ts_obs~as.Date(names(ts_obs)), col="grey4")
# points(ts_pred~as.Date(names(ts_pred)), col="blue", type="l", lwd=2)


dxl_cuts = cut(dat.ag$dxl, breaks = seq(0,300, by=10))
plot(x= head(filter(seq(0,300, by=10), filter = c(0.5,0.5)), -1), y=tapply(dat.ag$ba, INDEX = dxl_cuts, FUN = mean), col="green2", lwd=2, ylab="Burned area", xlab="Fuel mass")
# points(x= head(filter(seq(0,300, by=10), filter = c(0.5,0.5)), -1), y=tapply(dat.ag$ba.pred, INDEX = cut(dat.ag$dxl, breaks = seq(0,300, by=10)), FUN = mean), type="l", lwd=2, col="green4")

plot(tapply(dat.ag$ba, INDEX = cut(dat.ag$lmois, breaks = seq(0,1, by=0.05)), FUN = mean), col="cyan3", lwd=2, ylab="Burned area", xlab="Fuel moisture")
# points(tapply(dat.ag$ba.pred, INDEX = cut(dat.ag$lmois, breaks = seq(0,1, by=0.05)), FUN = mean), type="l", lwd=2, col="blue")

plot(tapply(dat.ag$ba, INDEX = cut(dat.ag$ts, breaks = seq(250,320, by=5)), FUN = mean), col="orange2", lwd=2, ylab="Burned area", xlab="Temperature")
# points(tapply(dat.ag$ba.pred, INDEX = cut(dat.ag$ts, breaks = seq(250,320, by=5)), FUN = mean), type="l", lwd=2, col="red")

# dev.off()

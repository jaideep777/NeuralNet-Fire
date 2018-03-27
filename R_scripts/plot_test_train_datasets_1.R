dat.ag.test = dat_eval
dat.ag. = dat_eval

plot.data.chars(dtrain, dtest){

  par(mfrow = c(3,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
  # 
  # plot(dat.ag$ba.pred~dat.ag$lmois, col=rgb(1-dat.ag$dxl/max(dat.ag$dxl), dat.ag$dxl/max(dat.ag$dxl), 0, alpha = 0.5), pch=20)
  # plot(dat.ag$ba~dat.ag$lmois, col=rgb(1-dat.ag$dxl/max(dat.ag$dxl), dat.ag$dxl/max(dat.ag$dxl), 0, alpha = 0.5), pch=20)
  # 
  # plot(dat.ag$ba.pred~dat.ag$dxl, col=rgb(1-dat.ag$lmois, 0, dat.ag$lmois))
  # plot(dat.ag$ba~dat.ag$dxl, col=rgb(1-dat.ag$lmois, 0, dat.ag$lmois))
  
  # plot(dat.ag$lmois~dat.ag$dxl, pch=20, cex=0.1, col=rgb(dat.ag$baclass_pred/max(dat.ag$baclass_pred), 1-dat.ag$baclass_pred/max(dat.ag$baclass_pred), 0))
  # points(dat.ag$lmois~dat.ag$dxl, pch=20, cex=dat.ag$baclass_pred/4, col=rgb(dat.ag$baclass_pred/max(dat.ag$baclass_pred), 1-dat.ag$baclass_pred/max(dat.ag$baclass_pred), 0))
  # plot(dat.ag$lmois~dat.ag$dxl, pch=20, cex=1, col=rgb(dat.ag$baclass_pred>0, dat.ag$baclass_pred==0, 0,alpha = 0.3))
  # par(mfrow = c(2,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
  plot(dat.ag$lmois~dat.ag$dxl, pch=20, cex=0.1, col=rgb(dat.ag$baclass/max(dat.ag$baclass), 1-dat.ag$baclass/max(dat.ag$baclass), 0))
  points(dat.ag$lmois~dat.ag$dxl, pch=20, cex=dat.ag$baclass/4, col=rgb(dat.ag$baclass/max(dat.ag$baclass), 1-dat.ag$baclass/max(dat.ag$baclass), 0))
  # plot(dat.ag$lmois~dat.ag$dxl, pch=20, cex=1, col=rgb(dat.ag$baclass>0, dat.ag$baclass==0, 0,alpha = 0.3))
  
  
  # plot(dat.ag$lmois~dat.ag$ts, pch=20, cex=0.1, col=rgb(dat.ag$baclass_pred/max(dat.ag$baclass_pred), 1-dat.ag$baclass_pred/max(dat.ag$baclass_pred), 0), xlim=c(270,320))
  # points(dat.ag$lmois~dat.ag$ts, pch=20, cex=dat.ag$baclass_pred/4, col=rgb(dat.ag$baclass_pred/max(dat.ag$baclass_pred), 1-dat.ag$baclass_pred/max(dat.ag$baclass_pred), 0))
  # plot(dat.ag$lmois~dat.ag$dxl, pch=20, cex=1, col=rgb(dat.ag$baclass_pred>0, dat.ag$baclass_pred==0, 0,alpha = 0.3))
  # par(mfrow = c(2,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
  plot(dat.ag$lmois~dat.ag$ts, pch=20, cex=0.1, col=rgb(dat.ag$baclass/max(dat.ag$baclass), 1-dat.ag$baclass/max(dat.ag$baclass), 0), xlim=c(250,320))
  points(dat.ag$lmois~dat.ag$ts, pch=20, cex=dat.ag$baclass/4, col=rgb(dat.ag$baclass/max(dat.ag$baclass), 1-dat.ag$baclass/max(dat.ag$baclass), 0))
  # plot(dat.ag$lmois~dat.ag$dxl, pch=20, cex=1, col=rgb(dat.ag$baclass>0, dat.ag$baclass==0, 0,alpha = 0.3))
  
  # plot(dat.ag$dxl~dat.ag$ts, pch=20, cex=0.1, col=rgb(dat.ag$baclass_pred/max(dat.ag$baclass_pred), 1-dat.ag$baclass_pred/max(dat.ag$baclass_pred), 0), xlim=c(270,320))
  # points(dat.ag$dxl~dat.ag$ts, pch=20, cex=dat.ag$baclass_pred/4, col=rgb(dat.ag$baclass_pred/max(dat.ag$baclass_pred), 1-dat.ag$baclass_pred/max(dat.ag$baclass_pred), 0))
  # plot(dat.ag$lmois~dat.ag$dxl, pch=20, cex=1, col=rgb(dat.ag$baclass_pred>0, dat.ag$baclass_pred==0, 0,alpha = 0.3))
  # par(mfrow = c(2,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
  plot(dat.ag$dxl~dat.ag$ts, pch=20, cex=0.1, col=rgb(dat.ag$baclass/max(dat.ag$baclass), 1-dat.ag$baclass/max(dat.ag$baclass), 0), xlim=c(250,320))
  points(dat.ag$dxl~dat.ag$ts, pch=20, cex=dat.ag$baclass/4, col=rgb(dat.ag$baclass/max(dat.ag$baclass), 1-dat.ag$baclass/max(dat.ag$baclass), 0))
  # plot(dat.ag$lmois~dat.ag$dxl, pch=20, cex=1, col=rgb(dat.ag$baclass>0, dat.ag$baclass==0, 0,alpha = 0.3))

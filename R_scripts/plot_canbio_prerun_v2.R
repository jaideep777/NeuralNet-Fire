#### PLOT PRERUN ####
fire_dir = "/home/jaideep/codes/FIRE_CODES"

dat = read.csv(file = paste0(fire_dir,"/fire_calcFuel/output_ssaplus_good/point_run.txt"), sep="\t",header=T, skip=4)

plot.canbio = function(y,x, label, col.pr="green3", col.main="black", col.pr1="green4"){
  plot(y~x, type="l", ylab=label, xaxt="n", col=col.main)
  pr = which(as.Date(dat$date) < as.Date("2005-4-1"))
  pr1 = which(as.Date(dat$date) > as.Date("2005-4-1") & as.Date(dat$date) < as.Date("2007-1-1"))
  points(y[pr]~x[pr], type="l", col=col.pr, lwd=2)
  points(y[pr1]~x[pr1], type="l", col=col.pr1, lwd=2)
}


png(paste0(fire_dir,"/figures/canbio_gpp_modis_24.png"), height=900*3, width=600*3, res=300)
par(mfrow = c(8,1), mar=c(0.2,5,1,1), oma=c(4,1,1,1))

# plot(dat$canbio0~as.Date(dat$date), type="l")
# points(dat$canbio0[1:60]~as.Date(dat$date[1:60]), type="l", col="green3", lwd=2)

plot.canbio(dat$canbio1, as.Date(dat$date), "NLE")
plot.canbio(dat$canbio2, as.Date(dat$date), "BLE")
# plot.canbio(dat$canbio3, as.Date(dat$date), "NLD")
# plot.canbio(dat$canbio4, as.Date(dat$date), "BLD")
plot.canbio(dat$canbio5, as.Date(dat$date), "SCD")
# plot.canbio(dat$canbio6, as.Date(dat$date), "SCX")
plot.canbio(dat$canbio7, as.Date(dat$date), "MD")
plot.canbio(dat$canbio8, as.Date(dat$date), "DD")
plot.canbio(dat$canbio9, as.Date(dat$date), "GR")
plot.canbio(dat$canbio10, as.Date(dat$date), "AG")
plot.canbio(dat$dxl, as.Date(dat$date), "dxL", col.main="brown", col.pr="yellow", col.pr1="orange")
axis(side=1, at = as.Date(dat$date), labels = substr(as.Date.character(dat$date),start = 1, stop = 4), tick = F)

dev.off()



#### NEURAL NET TEST ####
# 
# X = c(296.257, 105.228, 0.191869)
# Wi = matrix(nrow=3, data = c( 0.0338092381,  0.,     0.,   
#                               0.,     0.0077735282,  0.,   
#                               0.,     0.,     1.0909026166), byrow = T)
# bi =  c(-10.025,   -0.464,   -0.426)
# 
# W1 = matrix(nrow=3, data=c(-3.229, -1.299, -2.906, -0.672, -1.624,
#                            -3.178, -3.528, -1.885, -3.518, -3.526,
#                            1.995,  2.232,  1.747,  2.692,  3.071), byrow = T)
# 
# b1 = c(0.524,  0.227,  1.028,  0.029,  0.274)
# 
# Wo = matrix(nrow=5, data=c( 1.275,  0.973,  0.728, -0.377,  0.016, -0.774, -0.19,  -0.709,
#                             0.767, -0.035,  0.711,  0.33,  -0.667, -0.621, -0.328, -0.902,
#                             0.998,  0.387,  0.273,  0.445,  0.066, -0.427, -0.735, -2.07, 
#                             1.822,  0.171, -0.34,   0.187, -0.196, -1.072, -0.606, -0.968,
#                             0.771,  0.181, -0.469, -1.48,  -0.794, -1.3 ,  -0.845, -0.453), byrow = T)
# 
# bo = c(-0.393, -0.333,  0.103,  0.89,   0.354,  0.239, -1.616, -1.642)
# 
# X1 = X
# X1 = X1%*%Wi+bi
# X1 = 1/(1+exp(-(X1%*%W1+b1)))
# X1 = X1%*%Wo+bo
# Y = exp(X1)/sum(exp(X1))
# 
# 

  
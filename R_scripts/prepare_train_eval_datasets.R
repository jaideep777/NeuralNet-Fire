# Simulation name ("" or "india" or "ssaplus" etc)
sim_name           <- "ssaplus"

# Directories to the fire model folder
fire_dir           <- "/home/jaideep/codes/FIRE_CODES" # root directory for fire codes

#### Init ####
suffix = ""
if (sim_name != "") suffix = paste0(suffix,"_",sim_name)
output_dir = paste0("output",suffix)

source(paste0(fire_dir,"/R_scripts/utils.R"))

# clean up aggregated data in R and select forest grids only
datm = read.delim(paste0(fire_dir, "/fire_aggregateData/",output_dir,"/train_data.txt"), header=T)
# datm$msk = 1
if (sim_name == "ssaplus") datm$msk = 1
if (sim_name == "sas") datm$msk = 1
datm$msk[datm$msk == 0 | is.na(datm$msk)] = NA
datm[datm==9.9e20] = NA
datm = datm[,-length(datm)]
# datm$ba = datm$ba / 27.75e3^2
# fire_classes = c(0,1,4,16,64,256,1024)
# fire_classes = c(0,2^(seq(log2(2^0), log2(2^10), length.out=11)))
ba_classes = c(0,2^(seq(log2(2^0), log2(2^10), length.out=11)))/1024
# datm$fireclass = sapply(datm$ffev,FUN = function(x){length(which(x>fire_classes))})
datm$baclass = sapply(datm$ba,FUN = function(x){length(which(x>ba_classes))})
datm$gfedclass = sapply(datm$gfed,FUN = function(x){length(which(x>ba_classes))})
datm$date = as.Date(paste(datm$year,datm$month,datm$day, sep = "-"))
datm$logpop = log(1+datm$pop)

threshold_forest_frac = 0.3

dat_bad = datm[!complete.cases(datm),]
dat_good = datm[complete.cases(datm),]
datf = dat_good[dat_good$forest_frac > threshold_forest_frac,]
# datf = dat_good

xlim = c(60.5,100.5)
ylim = c(5,50)
ptsiz = 9  # 12 for india

png(paste0(fire_dir, "/fire_aggregateData/output",suffix,"/lmois.png"), width = 400, height = 500)
par(mfrow = c(1,2), cex.lab=1.2, cex.axis=1.2)
with( dat_good[dat_good$date == as.Date("2007-01-07"),],
      plot.colormap(X=lon, Y=lat, Z = lmois, zlim = c(-0.01,1.01), col = rainbow(100), cex = ptsiz, xlim = xlim, ylim = ylim)
)
dev.off()
png(paste0(fire_dir, "/fire_aggregateData/output",suffix,"/dft.png"), width = 400, height = 500)
par(mfrow = c(1,2), cex.lab=1.2, cex.axis=1.2)
with( dat_good[dat_good$date == as.Date("2007-01-07"),],
      plot.colormap1(X=lon, Y=lat, Z = dft, zlim = c(0,11), col = rainbow(12), cex = ptsiz, xlim = xlim, ylim = ylim)
)
dev.off()
png(paste0(fire_dir, "/fire_aggregateData/output",suffix,"/logpop.png"), width = 400, height = 500)
par(mfrow = c(1,2), cex.lab=1.2, cex.axis=1.2)
with( dat_good[dat_good$date == as.Date("2007-01-07"),],
      plot.colormap(X=lon, Y=lat, Z = logpop, zlim = c(-0.01,11), col = rainbow(100), cex = ptsiz, xlim = xlim, ylim = ylim)
)
dev.off()
png(paste0(fire_dir, "/fire_aggregateData/output",suffix,"/dxl.png"), width = 400, height = 500)
par(mfrow = c(1,2), cex.lab=1.2, cex.axis=1.2)
with( dat_good[dat_good$date == as.Date("2007-01-07"),],
      plot.colormap(X=lon, Y=lat, Z = dxl, zlim = c(-0.01,300), col = rainbow(100)[1:50], cex = ptsiz, xlim =xlim, ylim = ylim)
)
dev.off()

pos = which(datf$baclass>0)
neg = which(datf$baclass == 0)
neg_sub = sample(neg, size = 4*length(pos), replace = F)

set.seed(1)
ids = sample(c(pos, neg_sub), size = length(c(pos, neg_sub)), replace = F) # shuffle indices

datf = datf[ids,]
png(paste0(fire_dir, "/fire_aggregateData/output",suffix,"/dft_datf.png"), width = 400, height = 500)
par(mfrow = c(1,2), cex.lab=1.2, cex.axis=1.2)
with( datf[dat_good$date == as.Date("2007-01-07"),],
      plot.colormap1(X=lon, Y=lat, Z = dft, zlim = c(0,11), col = rainbow(12), cex = ptsiz, xlim = xlim, ylim = ylim)
)
dev.off()

plot.cut.means_obs = function(obs, var, min, max, col.obs, col.pred, ...){
  brks = seq(min,max, length.out=21)
  cuts = cut(var, breaks = brks, include.lowest = T)
  plot(x= mids(brks), y=tapply(obs, INDEX = cuts, FUN = mean), col=col.obs, lwd=2, ... ,ylim = c(0,0.020))
  list(classsizes = tapply(obs, INDEX = cuts, FUN = length), 
       classvalues = tapply(obs, INDEX = cuts, FUN = mean),
       class = cuts
  )
}



# datf$baclass = datf$baclass -1

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

ids_test = which(datf$date >= as.Date("2008-1-1") & datf$date <= as.Date("2010-12-31"))
# ids_test = which(datf$date >= as.Date("2013-1-1"))

dat_test = datf[ids_test,]
datf_train_full = datf[-ids_test,]

lt = dim(datf_train_full)[1]
id_train = sample(1:lt, size = 0.7*lt, replace = F)
id_eval = (1:lt)[-id_train]

dat_train = datf_train_full[id_train, ]
dat_eval = datf_train_full[id_eval, ]

### oversample rare forest types 
tt = table(dat_train$dft)
sample_size = max(tt)


# #### Remove spurious values from training set
# nl = plot.cut.means_obs(obs = dat_train$ba, var = dat_train$lmois, min = 0, max = 1, col.obs = "cyan3", col.pred = "blue", xlab="Fuel moisture", ylab="Burned area")
# nt = plot.cut.means_obs(obs = dat_train$ba, var = dat_train$ts, min = 250, max = 320, col.obs = "orange2", col.pred = "red", xlab="Fuel moisture", ylab="Burned area")
# nr = plot.cut.means_obs(obs = dat_train$ba, var = dat_train$rh, min = 0, max = 110, col.obs = "magenta", col.pred = "magenta4", xlab="Rel humidity", ylab="Burned area")
# nw = plot.cut.means_obs(obs = dat_train$ba, var = dat_train$wsp, min = 0, max = 8, col.obs = rgb(.3,.3,.3), col.pred = rgb(.6,.6,.6), xlab="Wind speed", ylab="Burned area")
# nh = plot.cut.means_obs(obs = dat_train$ba, var = dat_train$logpop, min = 0, max = 8, col.obs = "goldenrod", col.pred = "goldenrod4", xlab="Log pop density", ylab="Burned area")
# 
# dat_train$ba[which(nt$class == names(nt$classsizes[18]) | nt$class == names(nt$classsizes[17]))] = NA
# dat_train$ba[which(nl$class == names(nl$classsizes[1]))] = NA
# dat_train = dat_train[complete.cases(dat_train),]
# 
# dat_eval$ba[which(dat_eval$lmois < 0.05)] = NA
# dat_eval$ba[which(dat_eval$ts > 305)] = NA
# dat_eval = dat_eval[complete.cases(dat_eval),]


write.csv(x = dat_train, file=paste0(fire_dir, "/fire_aggregateData/",output_dir,"/train_forest.csv"), row.names = F)
write.csv(x = dat_eval, file=paste0(fire_dir, "/fire_aggregateData/",output_dir,"/eval_forest.csv"), row.names = F)
write.csv(x = dat_test, file=paste0(fire_dir, "/fire_aggregateData/",output_dir,"/test_forest.csv"), row.names = F)


datz = read.csv(file=paste0(fire_dir, "/fire_aggregateData/",output_dir,"/train_forest.csv"), header = T)
png(paste0(fire_dir, "/fire_aggregateData/output",suffix,"/lmois_datz.png"), width = 400, height = 500)
par(mfrow = c(1,2), cex.lab=1.2, cex.axis=1.2)
with( datz, #[as.Date(datz$date) == as.Date("2007-01-07"),],
      plot.colormap(X=lon, Y=lat, Z = lmois, zlim = c(-0.01,1.01), col = rainbow(100), cex = ptsiz, xlim = xlim, ylim = ylim)
)
dev.off()

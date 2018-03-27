datm = read.delim("/home/jaideep/codes/fire_aggregateData/output/train_data.txt", header=T)
datm[datm==9.9e20] = NA
datm = datm[,-length(datm)]
fire_classes = c(0,1,4,16,64,256,1024)
datm$fireclass = sapply(datm$ffev,FUN = function(x){length(which(x>fire_classes))})

threshold_forest_frac = 0.3

dat_bad = datm[!complete.cases(datm),]
dat_good = datm[complete.cases(datm),]
datf = dat_good[dat_good$forest_frac > threshold_forest_frac,]

write.csv(x = datf,file = "/home/jaideep/codes/fire_tensorflow/train_forest.csv")



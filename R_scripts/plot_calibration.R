# rm(list = ls())
# source(paste0(fire_dir,"/R_scripts/utils.R"))
#### PREDICTED FIRES - CALIBRATION ####

fire_dir = "/home/jaideep/codes/FIRE_CODES"

sim_name           <- "ssaplus/gfed_xcf_9"
suffix = ""
if (sim_name != "") suffix = paste0(suffix,"_",sim_name)
output_dir = paste0("output",suffix)

dataset = "eval"
system("mkdir figures", ignore.stderr = T)

# datf_train = read.fireData(dataset = "train", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
# datf_eval = read.fireData(dataset = "eval", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))
# datf_test = read.fireData(dataset = "test", dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))

datf = read.fireData_gfed(dataset = dataset, dir=paste0(fire_dir, "/fire_aggregateData/",output_dir))

#### calibration ####

# setwd(paste0("/home/jaideep/codes/FIRE_CODES/figures/",dataset))

plot_calib = function(datf, name, min=2e-4, max=2e-1, nscale = 200){
  tot.ba.pred = sum(datf$ba.pred)
  tot.ba.obs = sum(datf$ba)
  
  insuff_data = which(table(datf$baclass_pred)<10)
  for (i in 1:length(insuff_data)){
    datf$baclass_pred[datf$baclass_pred == as.numeric(names(insuff_data[i]))] = NA
  }
  datf = datf[complete.cases(datf),]
  
  f = function(x){
    # log(0.001+x)
    x
  }
  
#  par(mfrow = c(1,2), mar=c(4,4,1,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5)
  obs_ba.predc = tapply(X = datf$ba, INDEX = datf$baclass_pred, FUN=mean)
  pred_ba.predc = tapply(X = datf$ba.pred, INDEX = datf$baclass_pred, FUN=mean)
  obs_ba.predc = obs_ba.predc[-1]
  pred_ba.predc = pred_ba.predc[-1]
  n.obs = tapply(X = datf$ba.pred, INDEX = datf$baclass_pred, FUN=length)
  
  nscale = sum(n.obs)/120
  plot(obs_ba.predc~pred_ba.predc, log="xy", xlab = "Classwise mean\npredicted BA", ylab = "Classwise mean\nobserved BA", xlim=c(min,max), ylim=c(min,max), cex=1.5, lwd=2)
  points(obs_ba.predc~pred_ba.predc, cex=n.obs[-1]/nscale, pch=20, col=addTrans("black",trans = 30))
  abline(0,1,col="red", lwd=2)
  
  #a = summary(lm(obs_ba.predc~pred_ba.predc))
  #mtext(text = sprintf("r = %.2f", a$adj.r.squared), cex=1.5, side=3, adj = 0.1, padj = 2)
  nmse = 1-sum(log(obs_ba.predc[-1])-log(pred_ba.predc[-1]))^2/var(log(obs_ba.predc[-1]))/(length(obs_ba.predc[-1])-1)
  r = cor(obs_ba.predc, pred_ba.predc)
  mtext(text = sprintf("E = %.2f", nmse), cex=1., side=3, adj = 0.1, padj = 2, col="blue")
  mtext(text = sprintf("r = %.2f", r), cex=1., side=3, adj = 0.1, padj = 4, col="blue")
  
  mtext(col="blue",text = paste(name, "(n = ", nrow(datf),", obs = ",sprintf("%.2f",sum(datf$ba)*55.5e3^2*1e-10), " Mha, pred = ",sprintf("%.2f",sum(datf$ba.pred)*55.5e3^2*1e-10)," Mha)"), cex=1.1, side=3, adj = -0., padj = -1.5)  
  
  plot(f(datf$ba)~f(datf$ba.pred), pch=20, cex=0.2, xlab = "Predicted BA", ylab = "Observed BA", xlim=c(0,0.02), ylim=c(0,0.02))
  # abline(lm(f(datf$ba)~f(datf$ba.pred)), lwd=3)
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



setwd(paste0(fire_dir,"/fire_aggregateData/output",suffix,"/figures" ))


pfts_ssaplus = c(0, 1, 6, 10, 2, 7, 9, 11)
pftnames_ssaplus = c("Barren", "NLE", "SCX", "AGR", "BLE", "MD", "GR", "MX")


png(filename = "calib_all.png", width = 300*6, height = 500*6, res=300)
par(mfrow = c(4,2), mar=c(5,7,4,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5, mgp=c(4,1,0))
plot_calib(datf, "ALL", nscale=100)  # MIXED
dev.off()

png(filename = "PFTwise_1.png", width = 300*6, height = 500*6, res=300)
par(mfrow = c(4,2), mar=c(5,7,4,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5, mgp=c(4,1,0))
for (i in 1:4){
  plot_calib(datf[datf$dft==pfts_ssaplus[i],], pftnames_ssaplus[i], nscale=50)  # X
}
dev.off()

png(filename = "PFTwise_2.png", width = 300*6, height = 500*6, res=300)
par(mfrow = c(4,2), mar=c(5,7,4,1), oma=c(1,1,1,1), cex.axis=1.5, cex.lab=1.5, mgp=c(4,1,0))
for (i in 5:8){
  plot_calib(datf[datf$dft==pfts_ssaplus[i],], pftnames_ssaplus[i], nscale = 50)  # X
}
dev.off()




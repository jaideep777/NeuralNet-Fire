setwd("/home/jaideep/codes/supporting_codes_fire/codes_char_fixation_rates/ssaplus")#change according to your requirement
p=paste("mon_",0:11,".txt",sep="")

npft = 11

fix_rates = {}

for (mon in 1:12){
  dat = read.table(p[mon], header = T)
  mod<-lm(paste0("npp~",paste(names(dat)[5:length(dat)], collapse="+"), "-1"), data = dat)
  fix_rates = rbind(fix_rates, (as.numeric(summary(mod)$coefficients[,1])*24*(365.2524/12)))
}

colnames(fix_rates) = names(dat)[5:length(dat)]
write.csv(x = fix_rates, file="fix_rates_11levs.csv")


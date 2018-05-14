library(cffdrs)
dat = read.table("/home/jaideep/codes/FIRE_CODES/supporting_codes/nc2ascii_generic/ascii.txt",header= TRUE)
dat1 = dat[dat$msk > 0.5 & dat$msk < 9e20 & dat$ftmask >0.5 & dat$ftmask <9e20,]
dat2 = dat1[dat1$lat < 30,]

year = as.numeric(substr(as.Date(as.character(dat2$date)) , start = 1, stop = 4))
month = as.numeric(substr(as.Date(as.character(dat2$date)) , start = 6, stop = 7))
day = as.numeric(substr(as.Date(as.character(dat2$date)) , start = 9, stop = 10))

id = (dat2$lat *10000 + dat2$lon)*100

dat2_sample = dat2[which(id==11257625),]

ordered_dat = data.frame("id"= id,"lat"=dat2$lat,"long"= dat2$lon,"yr"=year,"mon"= month,"day"=day,
                          "temp"=dat2$ts-273.15,"rh"=dat2$rh,"ws"= dat2$wsp * 3.6,"prec"=dat2$pr)
ordered_sample = ordered_dat[which(id == 11257625),]
fwi.sample = fwi(ordered_dat)
spatial = tapply(X = fwi.sample$FWI,INDEX = fwi.sample$ID,FUN = mean )
# temporal = tapply(X = fwi.sample$FWI, INDEX = as.Date(fwi.sample$YR,fwi.sample$MON,fwi.sample$DAY,format = "%Y%M%D"), FUN = mean)
id_spatial = as.numeric(names(spatial))
lat = as.integer(id_spatial/10000)/100
lon=as.integer(id_spatial%%10000)/100

png(filename = "~/codes/FIRE_CODES/figures/fwi.png", width = 600*2, height = 500*2, res = 154)
plot.colormap(X = lon, Y = lat, Z = spatial, zlim = c(0,50), col = rainbow(100)[seq(80,1,-1)], cex = 14, xlim=c(60.25,99.75), ylim=c(5.25,30.25))
dev.off()
fwi = data.frame(lon=lon, lat=lat, fwi = spatial)

fire_obs = NcCreateOneShot(filename = "~/codes/FIRE_CODES/fire_aggregateData/output_sas/fire_obs_masked_2007-2015.nc", var_name = "ba", glimits = glimits)
fire_obs = NcClipTime(fire_obs, "2007-1-1", "2007-12-31")
fire_obs$data = fire_obs$data/55.5e3/55.5e3
slice_2007 = apply(X = fire_obs$data, MARGIN = c(1,2), FUN = mean)
rownames(slice_2007) = fire_obs$lons
colnames(slice_2007) = fire_obs$lats
baobs = melt(data = slice_2007, na.rm = T)
names(baobs) = c("lon", "lat", "ba")
plot.colormap(X = baobs$Var1, Y = baobs$Var2, Z = baobs$value, zlim = c(0,1), col = rainbow(100)[seq(80,1,-1)], cex = 10, xlim=c(60.25,99.75), ylim=c(5.25,30.25))

comb = merge(baobs, fwi, by=c("lon", "lat"))
cor(comb$fwi, comb$ba)


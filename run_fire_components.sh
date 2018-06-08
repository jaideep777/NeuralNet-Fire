#!/bin/bash
for i in 1 2 3 4 5 6 7 8 9 10; do
FILE=gfed_xlmois_$i
#cd fire_calcFuel
#./fire ssaplus
#cd ..
#cd fire_aggregateData
#mkdir output_ssaplus
#./aggregate train ssaplus
#cd ..
#Rscript R_scripts/prepare_train_eval_datasets.R
cd fire_tensorflow
#export PATH=$PATH:/home/jaideep/anaconda2/bin
./runtf 
cd ../fire_aggregateData
make
./aggregate eval ssaplus
cd ..
#Rscript R_scripts/all_plots.R
cd fire_aggregateData/output_ssaplus
cdo ifthen /media/jaideep/WorkData/Fire_G/forest_type/MODIS/ftmask_MODIS_0.5deg.nc fire.2007-1-1-2015-12-31.nc fire_pred_masked.nc
mkdir $FILE
mv fire.2007-1-1-2015-12-31.nc fire_pred_masked.nc weights_ba.txt y_predic*.txt ce_and_accuracy.txt $FILE
cd ../..
done


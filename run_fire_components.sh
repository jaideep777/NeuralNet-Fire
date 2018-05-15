#!/bin/bash
FILE=$1
#cd fire_calcFuel
#./fire ssaplus
#cd ..
#cd fire_aggregateData
#mkdir output_ssaplus
#./aggregate train ssaplus
#cd ..
#Rscript R_scripts/prepare_train_eval_datasets.R
cd fire_tensorflow
export PATH=$PATH:/home/jaideep/anaconda2/bin
./runtf 
cd ../fire_aggregateData
./aggregate eval ssaplus
cd ..
Rscript R_scripts/all_plots.R
cd fire_aggregateData/output_ssaplus
mkdir $FILE
mv figures fire.2007-1-1-2015-12-31.nc fire_pred_masked.nc weights_ba.txt y_predic*.txt ce_and_accuracy.txt $FILE
cd ../..


# Simulation name ("" or "india" or "ssaplus" etc)
sim_name           <- "ssaplus"

# Directories to the fire model folder
fire_dir           <- "/home/jaideep/codes/FIRE_CODES" # root directory for fire codes

#### Init ####
suffix = ""
if (sim_name != "") suffix = paste0(suffix,"_",sim_name)
output_dir = paste0("output",suffix)

source(paste0(fire_dir,"/R_scripts/utils.R"))
source(paste0(fire_dir,"/R_scripts/plot_aggregate_maps_timeseries.R"))
source(paste0(fire_dir,"/R_scripts/plot_calibration.R"))
source(paste0(fire_dir,"/R_scripts/plot_maps.R"))
source(paste0(fire_dir,"/R_scripts/plot_test_train_datasets_3.R"))

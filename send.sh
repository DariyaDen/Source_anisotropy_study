#!/bin/sh

# SLURM options:

#SBATCH --job-name=exercise_job
#SBATCH --mem=10G
#SBATCH --licenses=sps

#SBATCH --time=15:00
#SBATCH --cpus-per-task=1

FAL_DIR=/sps/nemo/sw/redhat-9-x86_64/snsw/opt2/falaise-5.1.5/bin
MY_DIR=/sps/nemo/scratch/ohalatova/kink_track_study/Bi-207
# SW_DIR=/sps/nemo/scratch/ohalatova/softwares

source ${THRONG_DIR}/config/supernemo_profile.bash  
snswmgr_load_setup falaise@5.1.5

${FAL_DIR}/flsimulate -c ${MY_DIR}/simu_setup.conf -o ${MY_DIR}/simu_Bi.brio

${FAL_DIR}/flreconstruct -i ${MY_DIR}/simu_Bi.brio -p ${MY_DIR}/reco.conf -o ${MY_DIR}/CM_reco_Bi.brio

${FAL_DIR}/flreconstruct -i ${MY_DIR}/CM_reco_Bi.brio -p ${MY_DIR}/calibration_cuts.conf -o ${MY_DIR}/EnCalib_Bi.brio

# ${FAL_DIR}/flreconstruct -i ${MY_DIR}/CM_reco_Bi.brio -p ${SW_DIR}/SNCuts/build/SNCutsPipeline.conf -o ${MY_DIR}/SNC_reco_Bi.brio

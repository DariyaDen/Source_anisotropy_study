#!/bin/sh

# SLURM options:
#SBATCH --job-name=newcuts
#SBATCH --mem=3000M
#SBATCH --licenses=sps
#SBATCH --time=1:30:00
#SBATCH --cpus-per-task=1

source ${THRONG_DIR}/config/supernemo_profile.bash  
snswmgr_load_setup falaise@5.1.5

FAL_DIR=/sps/nemo/sw/redhat-9-x86_64/snsw/opt2/falaise-5.1.5/bin
MY_DIR=/sps/nemo/scratch/ddenysenko/GE/kink-track-study---Oleksandra/Bi-207

num=$1

${FAL_DIR}/flreconstruct \
  -i ${MY_DIR}/DATA/$num/CM_reco_Bi_$num.brio \
  -p ${MY_DIR}/0cut.conf \
  -o ${MY_DIR}/DATA/$num/Default0cuts.root

# Run reconstruction
#${FAL_DIR}/flreconstruct \
#  -i ${MY_DIR}/DATA/$num/CM_reco_Bi_$num.brio \
#  -p ${MY_DIR}/SNCutsAndMiModule.conf \
#  -o ${MY_DIR}/DATA/$num/reco_Bi_$num.brio

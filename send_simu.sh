#!/bin/sh

# SLURM options:

#SBATCH --job-name=Bi207
#SBATCH --mem=3500M

#SBATCH --licenses=sps

#SBATCH --time=3:30:00
#SBATCH --cpus-per-task=1

FAL_DIR=/sps/nemo/sw/redhat-9-x86_64/snsw/opt2/falaise-5.1.5/bin
MY_DIR=/sps/nemo/scratch/ddenysenko/GE/kink-track-study---Oleksandra/Bi-207

source ${THRONG_DIR}/config/supernemo_profile.bash  
snswmgr_load_setup falaise@5.1.5

num=$1

${FAL_DIR}/flsimulate -c ${MY_DIR}/simu_setup.conf -o ${MY_DIR}/DATA/$num/simu_Bi_$num.brio

${FAL_DIR}/flreconstruct -i ${MY_DIR}/DATA/$num/simu_Bi_$num.brio -p ${MY_DIR}/reco.conf -o ${MY_DIR}/DATA/$num/CM_reco_Bi_$num.brio

rm -f ${MY_DIR}/DATA/$num/simu_Bi_$num.brio     # -f ensures it will not complain if file doen't exist

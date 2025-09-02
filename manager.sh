#!/bin/sh

DATA_DIR="/sps/nemo/scratch/ddenysenko/GE/kink-track-study---Oleksandra/Bi-207"

mkdir DATA
cd DATA

first_val=$1
second_val=$2
name_of_script=$3

for ((num=first_val; num<=second_val; num++))
do 
	mkdir -p "$num"
	cd $num
	
	cp ../../simu.profile simu.profile
	cp ../../reco.conf reco.conf
	cp ../../pipeline.conf pipeline.conf
	cp ../../SNCutsAndMiModule.conf SNCutsAndMiModule.conf
	cp ../../simu_setup.conf simu_setup.conf
		
		sbatch \
		--output="${DATA_DIR}/DATA/$num/slurm-%j.out" \
		../../"$name_of_script" "$num"
	cd ..	
	
done


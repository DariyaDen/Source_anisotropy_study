# GeometricEfficiencies
Geometric efficiency for superNEMO OM-Bismuth source pairs

This project revolves around the calculation and visualization of geometrical efficiencies of main wall optical module-Bismuth source pairs in superNEMO experiment.

source_positions.txt.in: contains positions of the 42 Bismuth sources as provided by Filip's CalibrationModule
source_positions.txt: Same, but have one more collumn with numeration of sources

source_activity.txt.in: contains activities associated with each Bismuth source and their associated numbers as numbered by superNEMO (not explicitly used in the code)
0m_positions.txt: contains positions of the 260 optical modules

manager.sh:


This script is the controller of the whole workflow.
It creates directories for each job,

copies configuration files into them,

submits batch jobs to SLURM using the chosen script (send_simu.sh or send_cuts.sh).


simu.profile:


Defines the detector layout, source material, source thickness, calibration, and simulation settings.

Examples of parameters:

layout/if_basic/source_layout/thickness = 250 um

tracking_gas_material = "Nemo3"

generator = "calibBi207" → particle generator for Bi-207.

This file is read by the simulator to know what physics and geometry to simulate.


simu_setup.conf:


Controls the simulation run:

Number of events to generate (numberOfEvents = 1000000).

Which variant profile to use (points to simu.profile).


send_simu.sh:


Shell script to submit the simulation job via SLURM (cluster job scheduler).

Arguments:

$1 = directory number to store the output (for batch simulations).

Commands executed:

flsimulate → run simulation using simu_setup.conf

flreconstruct → basic reconstruction on the simulated events

Cleans up raw simulation files if needed

Example usage: sbatch send_simu.sh 0   # Run simulation for directories 0


reco.conf:


Defines the reconstruction pipeline:

Which plugins to load: Falaise_TKReconstruct, GammaTracking, Falaise_GammaClustering.

Variant profile to use: points to simu.profile.

Path to the pipeline config file (e.g., pipeline.conf).

Used by flreconstruct to reconstruct detector events.

reco0cut.conf:

Makes everything same like reco.conf, just without cuts.


SNCutsAndMiModule.conf:


Defines a custom reconstruction pipeline using:

SNCuts → applies standard cuts (energy, track criteria, proximity to source)

MiModule → handles custom module data

Contains cut parameters:

hasNumberofKinks = "1 0" → only accept events with one kinked track

useEventTrackHasOneAssocCaloHit = true → track must have one calorimeter hit

Allows flexible tuning of cuts for analysis.


0cut.conf:

Reconstruction without any cuts. Works like SNCutsAndMiModule.conf, is needed for simu_eff.cpp


send_cuts.sh:

SLURM shell script to run flreconstruct with SNCutsAndMiModule.conf.

Reads reconstructed events from simulation and applies cuts.

Outputs results as .brio ROOT files ready for efficiency calculation.

send.sh:

Optional: full workflow script combining simulation and reconstruction.

Can be used to run everything in one go.

simu_eff.cpp

Reads reconstructed events after cuts (.root files) and calculates detection efficiency for each source and OM.

Key steps:

Read source and OM positions from text files (source_positions.txt.in, om_positions.txt).

Loop over events:

Count how many events were generated per source (count_root_before)

Count how many events were detected in each OM (count_root_after)

Compute efficiency per OM and source.

Save efficiency maps for each source as histograms in simu_eff.root.

Also precomputes MainWall OM positions to map events to physical coordinates (Y,Z).


Example usage:

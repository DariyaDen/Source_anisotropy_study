# Geometric Efficiencies for Bi-207 Calibration Sources in SuperNEMO

This project computes and visualizes the **geometrical detection efficiencies** of the **Main Wall Optical Modules (OMs)** with respect to the **Bi-207 calibration sources** in the SuperNEMO experiment.

It combines **simulation, reconstruction, analysis, and visualization** into one workflow.

---

## 📂 Project Structure

### 1. Input Data Files

- **`source_positions.txt.in`**  
  Original list of Bi-207 source coordinates (from Filip’s CalibrationModule).  

- **`source_positions.txt`**  
  Same list, but reformatted: adds a first column with **source index (0–41)**.  
  → Used by analysis/plotting code.  

- **`source_activity.txt.in`**  
  Activities (strengths) of each source, paired with its ID.  
  → Used for weighting efficiency maps.  

- **`om_positions.txt`**  
  Coordinates of all **260 optical modules (OMs)** in the Main Wall.  

---

### 2. Simulation Configuration

- **`simu.profile`**  
  Defines detector geometry and simulation parameters:  
  - Source thickness, material, shielding  
  - Calibration setup (tracks to simulate)  
  - Particle generator (`calibBi207`)  
  - Physics mode (`Constructors`, EM model)  

- **`simu_setup.conf`**  
  Tells **flsimulate** how to run the simulation:  
  - `numberOfEvents = 1000000` (events per run)  
  - Path to `simu.profile`  

---

### 3. Reconstruction Configuration

- **`reco.conf`**  
  Standard reconstruction pipeline (uses plugins like `GammaTracking`).  

- **`reco0cut.conf`**  
  Same as above, but **no event cuts applied** (baseline reconstruction).  

- **`pipeline.conf`**  
  Defines the exact reconstruction pipeline used by `reco.conf`.  

- **`pipeline0cut.conf`**  
  Pipeline for `reco0cut.conf`.  

- **`0cut.conf`**  
  Another reconstruction config with **no cuts**, used for efficiency calculation in `simu_eff.cpp`.  

- **`SNCutsAndMiModule.conf`**  
  Custom reconstruction pipeline using:  
  - **`SNCuts`** → applies event selection (energy, track, proximity, etc.)  
  - **`MiModule`** → custom module for event handling  

  Example cuts:  
  - `hasNumberofKinks = "1 0"` → only accept events with one kinked track  
  - `useEventTrackHasOneAssocCaloHit = true` → one calo hit per track  

---

### 4. Job Management Scripts

- **`manager.sh`**  
  High-level controller script:  
  - Creates run directories (`DATA_2/0`, `DATA_2/1`, …).  
  - Copies all needed config files into them.  
  - Submits jobs via SLURM (`sbatch`).  
  - Arguments:  
    ```sh
    ./manager.sh 0 10 send_simu.sh
    ```
    → runs jobs for runs **0–10** using `send_simu.sh`.

- **`send_simu.sh`**  
  SLURM job script to run simulation **+ reconstruction**.  
  Steps:  
  1. `flsimulate` → generate events with `simu_setup.conf`.  
  2. `flreconstruct` → reconstruct with `reco.conf`.  
  3. Cleanup temporary `.brio` files.  

- **`send_cuts.sh`**  
  SLURM job script for reconstruction **with cuts**, using `SNCutsAndMiModule.conf`.  

- **`send.sh`**  
  Combined script to run **simulation + reconstruction + cuts** in one go.  
  (Optional; `manager.sh` usually controls the workflow.)  

---

### 5. Efficiency Calculation & Analysis

- **`simu_eff.cpp`**  
  ROOT C++ program to compute efficiencies:  
  - Reads reconstructed `.root` files.  
  - Counts how many events were generated per source (`N_before`).  
  - Counts how many events hit each OM (`N_after`).  
  - Efficiency = `N_after / N_before`.  
  - Saves per-source histograms (`hist0 … hist41`) into **`simu_eff.root`**.  

- **`total_eff.cpp`**  
  Computes the **global efficiency** (summing all sources and OMs).  
  Saves result in **`total_eff.root`**.  

- **`plotting.cpp`**  
  Helper plotting macro (alternative to `visu.cpp`).  
  → Can be used for quick diagnostic plots of histograms.  

---

### 6. Visualization

- **`visu.cpp`**  
  ROOT C++ program to create publication-quality figures:  
  - For each source: efficiency map (`eps_G_source_XX.png`).  
  - Source & OM positions map (`source_OM_map.png`).  
  - Weighted efficiency sum (all sources weighted by activity).  

- **`plots/`**  
  Output folder with generated PNGs.  

---

### 7. Output Files

- **`simu_eff.root`**  
  Contains per-source efficiency histograms (`hist0 … hist41`).  

- **`total_eff.root`**  
  Contains total/global efficiency histograms.  

---

## 🔄 Workflow Overview

1. **Prepare input files**  
   (`source_positions.txt`, `om_positions.txt`, `source_activity.txt.in`, configs).  

2. **Run jobs with manager**  
   ```sh
   ./manager.sh 0 5 send_simu.sh

# Geometric Efficiencies for Bi-207 Calibration Sources in SuperNEMO

This project computes and visualizes the **geometrical detection efficiencies** of the **Main Wall Optical Modules (OMs)** with respect to the **Bi-207 calibration sources** in the SuperNEMO experiment.

It combines **simulation, reconstruction, analysis, and visualization** into one workflow.

---

## 📂 Project Structure

## 🌿 Branch Overview

**main** – ✅ Complete implementation for calculating geometrical efficiencies and generating histograms.

**master** – 🚧 Work-in-progress branch for theoretical efficiencies (not fully implemented yet).

### 1. Input Data Files

- **`source_positions.txt.in`(exist in both branches)**  
  Original list of Bi-207 source coordinates (from Filip’s CalibrationModule).  

- **`source_positions.txt`(exist in both branches)**  
  Same list, but reformatted: adds a first column with **source index (0–41)**.  
  → Used by analysis/plotting code.  

- **`source_activity.txt.in`(exist in both branches)**  
  Activities (strengths) of each source, paired with its ID.  
  → Used for weighting efficiency maps.  

- **`om_positions.txt`(exist in both branches)**
  Coordinates of all **260 optical modules (OMs)** in the Main Wall.  

---

### 2. Simulation Configuration

- **`simu.profile`(branch master)**  
  Defines detector geometry and simulation parameters:  
  - Source thickness, material, shielding  
  - Calibration setup (tracks to simulate)  
  - Particle generator (`calibBi207`)  
  - Physics mode (`Constructors`, EM model)  

- **`simu_setup.conf`(branch master)**  
  Tells **flsimulate** how to run the simulation:  
  - `numberOfEvents = 1000000` (events per run)  
  - Path to `simu.profile`  

---

### 3. Reconstruction Configuration

- **`reco.conf`(branch master)**  
  Standard reconstruction pipeline (uses plugins like `GammaTracking`).  

- **`reco0cut.conf`(branch master)**  
  Same as above, but **no event cuts applied** (baseline reconstruction).  

- **`pipeline.conf`(branch master)**  
  Defines the exact reconstruction pipeline used by `reco.conf`.  

- **`pipeline0cut.conf`(branch master)**  
  Pipeline for `reco0cut.conf`.  

- **`0cut.conf`(branch master)**  
  Another reconstruction config with **no cuts**, used for efficiency calculation in `simu_eff.cpp`.  

- **`SNCutsAndMiModule.conf`(branch master)**  
  Custom reconstruction pipeline using:  
  - **`SNCuts`** → applies event selection (energy, track, proximity, etc.)  
  - **`MiModule`** → custom module for event handling  

  Example cuts:  
  - `hasNumberofKinks = "1 0"` → only accept events with one kinked track  
  - `useEventTrackHasOneAssocCaloHit = true` → one calo hit per track  

---

### 4. Job Management Scripts

- **`manager.sh`(branch master)**  
  High-level controller script:  
  - Creates run directories (`DATA_2/0`, `DATA_2/1`, …).  
  - Copies all needed config files into them.  
  - Submits jobs via SLURM (`sbatch`).  
  - Arguments:  
    ```sh
    ./manager.sh 0 10 send_simu.sh
    ```
    → runs jobs for runs **0–10** using `send_simu.sh`.

- **`send_simu.sh`(branch master)**  
  SLURM job script to run simulation **+ reconstruction**.  
  Steps:  
  1. `flsimulate` → generate events with `simu_setup.conf`.  
  2. `flreconstruct` → reconstruct with `reco.conf`.  
  3. Cleanup temporary `.brio` files.  

- **`send_cuts.sh`(branch master)**  
  SLURM job script for reconstruction **with cuts**, using `SNCutsAndMiModule.conf`.  

- **`send.sh`(branch master)**  
  Combined script to run **simulation + reconstruction + cuts** in one go.  
  (Optional; `manager.sh` usually controls the workflow.)  

---

### 5. Efficiency Calculation & Analysis

- **`simu_eff.cpp`(branch master)**  
  ROOT C++ program to compute efficiencies:  
  - Reads reconstructed `.root` files.  
  - Counts how many events were generated per source (`N_before`).  
  - Counts how many events hit each OM (`N_after`).  
  - Efficiency = `N_after / N_before`.  
  - Saves per-source histograms (`hist0 … hist41`) into **`simu_eff.root`**.  

- **`total_eff.cpp`(branch main)**  
  Computes the **global efficiency** (summing all sources and OMs).  
  Saves result in **`total_eff.root`**.  

- **`plotting.cpp`(branch main)**  
  Helper plotting macro (alternative to `visu.cpp`).  
  → Can be used for quick diagnostic plots of histograms.  

---

### 6. Visualization

- **`visu.cpp`(exist in both branches)**  
  ROOT C++ program to create publication-quality figures:  
  - For each source: efficiency map (`eps_G_source_XX.png`).  
  - Source & OM positions map (`source_OM_map.png`).  
  - Weighted efficiency sum (all sources weighted by activity).  

- **`plots/`(exist in both branches)**  
  Output folder with generated PNGs.  

---

### 7. Output Files

- **`simu_eff.root`(branch master)**  
  Contains per-source efficiency histograms (`hist0 … hist41`).  

- **`total_eff.root`(branch main)**  
  Contains total/global efficiency histograms.  

---

## 🔄 Workflow Overview

1. **Prepare input files**  
   (`source_positions.txt`, `om_positions.txt`, `source_activity.txt.in`, configs).  

2. **Run jobs with manager**  
   ```sh
   ./manager.sh 0 5 send_simu.sh

# GeometricEfficiencies
Geometric efficiency for superNEMO OM-Bismuth source pairs

This project revolves around the calculation and visualization of geometrical efficiencies of main wall optical module-Bismuth source pairs in superNEMO experiment.

total_eff.cpp: creates 2D histogram visualization of (1) total geometrical efficiencies multiplied by activities of the sources as measured in July 1 2018 by Miro and (2) geometrical efficiencies associated with a singular source

plotting.cpp: generates a plot of optical modules (as boxes) and sources (as green dots) according to superNEMO geometry

source_positions.txt.in: contains positions of the 42 Bismuth sources as provided by Filip's CalibrationModule

source_positions.txt: contains positions of the 42 Bismuth sources as provided by Filip's CalibrationModule

source_activity.txt.in: contains activities associated with each Bismuth source and their associated numbers as numbered by superNEMO (not explicitly used in the code)

om_positions.txt: contains positions of the 260 optical mosules

total_eff.root: file created by total_eff.cpp that contains all information about histograms but vosout visualization

visu.cpp: vizualisation of histogram, saving them as .png(you could see examples in dir plots)

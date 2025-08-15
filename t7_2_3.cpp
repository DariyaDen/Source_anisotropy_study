#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>
#include <TROOT.h>
#include <TApplication.h>
#include <iostream>

void t7_2_3(
    const std::string& outputFileName = "conductivity_experiment.root",
    unsigned int numDataPoints = 1000000
) {
    TFile ofile(outputFileName.c_str(), "RECREATE");
    if (ofile.IsZombie()) {
        std::cerr << "Error: could not create file " << outputFileName << std::endl;
        return;
    }

    // Initialise TTree
    TTree cond_data("cond_data", "Example N-Tuple with branches");

    // Define variables and branches
    float pot, cur, temp, pres;
    cond_data.Branch("Potential", &pot, "Potential/F");
    cond_data.Branch("Current", &cur, "Current/F");
    cond_data.Branch("Temperature", &temp, "Temperature/F");
    cond_data.Branch("Pressure", &pres, "Pressure/F");

    // Random generator
    TRandom3 rndm(0); // seed=0 for reproducibility

    // Fill the tree
    for (unsigned int i = 0; i < numDataPoints; ++i) {
        pot  = rndm.Uniform(0., 10.) * rndm.Gaus(1., 0.01);
        temp = rndm.Uniform(250., 350.) + rndm.Gaus(0., 0.3);
        pres  = rndm.Uniform(0.5, 1.5) * rndm.Gaus(1., 0.02);
        cur  = pot / (10. + 0.05*(temp - 300.) - 0.2*(pres - 1.)) * rndm.Gaus(1., 0.01);

        cond_data.Fill();
    }

    // Save and close
    cond_data.Write();
    ofile.Close();

    std::cout << "Data written to " << outputFileName << std::endl;
}

// --- Standalone main ---
int main(int argc, char** argv) {
    TApplication app("ROOT Application", &argc, argv);
    t7_2_3();
    return 0;
}


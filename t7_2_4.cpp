#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TApplication.h>
#include <iostream>   // for std::cout, std::endl
#include <cstddef>    // for size_t if needed

void t7_2_4() {
    // Create a TChain for all trees named "cond_data"
    TChain in_chain("cond_data");
    in_chain.Add("conductivity_experiment*.root"); // supports wildcards

    // Variables for branches
    float pot, cur, temp, pres;
    in_chain.SetBranchAddress("Potential", &pot);
    in_chain.SetBranchAddress("Current", &cur);
    in_chain.SetBranchAddress("Temperature", &temp);
    in_chain.SetBranchAddress("Pressure", &pres);

    std::cout << "Potential\tCurrent\tTemperature\tPressure" << std::endl;
    for (Long64_t irow = 0; irow < in_chain.GetEntries(); ++irow) {
        in_chain.GetEntry(irow); // load the current row
        std::cout << pot << "\t" << cur << "\t" << temp << "\t" << pres << std::endl;
    }
}

// Standalone execution
int main(int argc, char** argv) {
    TApplication app("ROOT Application", &argc, argv);
    t7_2_4();
    return 0;
}


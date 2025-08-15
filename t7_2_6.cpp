#include <TApplication.h>
#include <TChain.h>
#include <TProof.h>
#include <iostream>

int main(int argc, char** argv) {
    TApplication app("ROOT Application", &argc, argv);

    // === Create a TChain for all trees named "cond_data" ===
    TChain *ch = new TChain("cond_data", "My Chain for Example N-Tuple");
    ch->Add("conductivity_experiment*.root"); // supports wildcards

    // === Start PROOF Lite using 4 worker threads ===
    std::cout << "Starting PROOF Lite with 4 workers..." << std::endl;
    TProof::Open("workers=4");

    // === Enable PROOF for the chain ===
    ch->SetProof();

    // === Process the chain using the selector ===
    std::cout << "Processing chain with MySelector.C+ ..." << std::endl;
    ch->Process("MySelector.C+");

    std::cout << "Processing completed." << std::endl;

    return 0;
}


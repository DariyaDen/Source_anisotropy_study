#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>
#include <iostream>

void analysis() {
    // Create output file
    TFile outFile("data.root", "RECREATE");
    if (outFile.IsZombie()) {
        std::cerr << "Error: Could not create file data.root" << std::endl;
        return;
    }

    // Create a TTree and a branch for the random values
    TTree tree("tree", "Random uniform data");
    float value;
    tree.Branch("value", &value, "value/F");

    // Random number generator
    TRandom3 rnd(0); // seed=0 for random seed

    // Generate 10,000 random values
    for (int i = 0; i < 10000; ++i) {
        value = rnd.Uniform(0.0, 3500.0);
        tree.Fill();
    }

    // Save the tree to the file
    tree.Write();
    outFile.Close();

    std::cout << "10,000 random values saved to data.root" << std::endl;
}


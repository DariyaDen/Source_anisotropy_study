// t7_2_5_1.cpp
#include <TFile.h>
#include <TTree.h>
#include <iostream>

int t7_2_5_1() {
    // Open ROOT file containing the tree
    TFile f("conductivity_experiment.root");
    if (f.IsZombie()) {
        std::cerr << "Error: could not open file conductivity_experiment.root" << std::endl;
        return 1;
    }

    // Get the TTree from the file
    TTree* t = nullptr;
    f.GetObject("cond_data", t);
    if (!t) {
        std::cerr << "Error: TTree 'cond_data' not found in file." << std::endl;
        return 1;
    }

    // Generate a Selector for the tree
    // This will create MySelector.h and MySelector.C in the current directory
    t->MakeSelector("MySelector");

    std::cout << "Selector 'MySelector' has been generated." << std::endl;
    return 0;
}

// Standalone execution
int main() {
    return t7_2_5_1();
}


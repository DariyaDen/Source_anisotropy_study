#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TApplication.h>
#include <iostream>

int main(int argc, char** argv) {
    TApplication app("ROOT Application",&argc,argv);

    // Open the file
    TFile f("conductivity_experiment.root");
    if (f.IsZombie()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    // Get the tree
    TTree* t = nullptr;
    f.GetObject("cond_data", t);
    if (!t) {
        std::cerr << "Tree not found!" << std::endl;
        return 1;
    }

    // Variables
    float Potential, Current, Temperature, Pressure;
    t->SetBranchAddress("Potential", &Potential);
    t->SetBranchAddress("Current", &Current);
    t->SetBranchAddress("Temperature", &Temperature);
    t->SetBranchAddress("Pressure", &Pressure);

    // Histograms
    TH1F hPot("hPot", "Voltage distribution", 100, 0, 10);
    TH1F hCur("hCur", "Current distribution", 100, 0, 2);
    TH1F hTemp("hTemp", "Temperature distribution", 100, 250, 350);
    TH1F hPres("hPres", "Pressure distribution", 100, 0.5, 1.5);

    // Loop over entries safely
    Long64_t nentries = t->GetEntries();
    for (Long64_t i=0; i<nentries; ++i) {
        t->GetEntry(i);
        hPot.Fill(Potential);
        hCur.Fill(Current);
        hTemp.Fill(Temperature);
        hPres.Fill(Pressure);
    }

    // Draw and save
    TCanvas c1("c1","Voltage",800,600); hPot.Draw(); c1.SaveAs("Potential.png");
    TCanvas c2("c2","Current",800,600); hCur.Draw(); c2.SaveAs("Current.png");
    TCanvas c3("c3","Temperature",800,600); hTemp.Draw(); c3.SaveAs("Temperature.png");
    TCanvas c4("c4","Pressure",800,600); hPres.Draw(); c4.SaveAs("Pressure.png");

    std::cout << "Histograms saved as PNG files." << std::endl;
    return 0;
}


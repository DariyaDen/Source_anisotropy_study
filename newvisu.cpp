#include <TFile.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TStyle.h>

void newvisu() {
    // Open the ROOT file
    TFile* f = TFile::Open("simu_eff.root","READ");
    if(!f || f->IsZombie()) {
        std::cerr << "Cannot open simu_eff.root" << std::endl;
        return;
    }

    // Get the efficiency histogram
    TH2F* hEff = (TH2F*)f->Get("hEff");
    if(!hEff) {
        std::cerr << "Histogram hEff not found in simu_eff.root" << std::endl;
        f->Close();
        return;
    }

    // Optional: style
    gStyle->SetOptStat(0);       // turn off statistics box
    gStyle->SetPalette(kViridis); // nice color map

    // Draw the histogram
    TCanvas* c1 = new TCanvas("c1","Efficiency Heatmap",1200,600);
    hEff->SetTitle("Efficiency per Source vs OM; Source ID; OM ID");
    hEff->Draw("COLZ"); // color-coded 2D histogram

    // Save as PNG
    c1->SaveAs("/sps/nemo/scratch/ddenysenko/GE/kink-track-study---Oleksandra/Bi-207/plots1/simu_eff_heatmap.png");

    f->Close();
}


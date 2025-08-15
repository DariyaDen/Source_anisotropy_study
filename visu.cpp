#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <iostream>

void visu() {
double rl = 0.09, rr = 0.3, rt = 0.1, rb = 0.1;
	double px = 1000;
	double py = px * (1 - rl - rr) / (1 - rb - rt);
    TFile *file = TFile::Open("data.root");
    if (!file || file->IsZombie()) return;

    TTree *tree = (TTree*)file->Get("tree");
    if (!tree) return;

    Float_t value;
    tree->SetBranchAddress("value", &value);

    TH1D *hist = new TH1D("hist", "Random Values;Value;Counts", 100, 0, 3500);

    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; ++i) {
        tree->GetEntry(i);
        hist->Fill(value);
    }

TCanvas* c = new TCanvas("c", "Histogram of Values", px, py);

    gStyle->SetOptStat(1111); // show stats
        	c->SetLeftMargin(rl);
       		c->SetTopMargin(rt);
        	c->SetBottomMargin(rb);
        	c->SetRightMargin(rr);
        	c->cd();

        
        	hist->Draw();
}


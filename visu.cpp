#include <TFile.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TH2D.h>
#include <TGraph.h>
#include <TLatex.h>
#include <fstream>
#include <sstream>
#include <iostream>

// Reads one pair of values (x, y) from the specified line in the input file
std::pair<double, double> get_values_on_line(const std::string& filename, int target_line) {
    std::ifstream infile(filename);
    if (!infile) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    int current_line = 0;

    while (std::getline(infile, line)) {
        if (current_line == target_line) {
            std::istringstream ss(line);
            std::string token;
            double val1, val2;

            if (std::getline(ss, token, ';')) {
                val1 = std::stod(token);
            } else {
                throw std::runtime_error("Missing first value on line " + std::to_string(target_line));
            }

            if (std::getline(ss, token)) {
                val2 = std::stod(token);
            } else {
                throw std::runtime_error("Missing second value on line " + std::to_string(target_line));
            }

            return {val1, val2};
        }
        ++current_line;
    }

    throw std::out_of_range("Line number " + std::to_string(target_line) + " exceeds file length");
}

void visu() {
    // Open the ROOT file with histograms
    TFile *file = TFile::Open("total_eff.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: cannot open total_eff.root" << std::endl;
        return;
    }

    // Visualization options
    bool total_vis = false;  // set to true if you want only the "hist" histogram
    double mw_sizey = 256.0;
    double mw_sizez = 256.0;
    int nbinsx = 20;
    int nbinsy = 13;

    double rl = 0.09;
    double rr = 0.35;
    double rt = 0.1;
    double rb = 0.1;
    double R = double(nbinsx) / double(nbinsy);
    double px = 1000;
    double py = (px / R) * (1 - rl - rr) / (1 - rb - rt);

    // TOTAL VISUALIZATION
    if (total_vis) {
        TH2D *hist = (TH2D*)file->Get("hist");
        if (!hist) {
            std::cerr << "Error: 'hist' not found in file" << std::endl;
            return;
        }

        TCanvas *c = new TCanvas("c", "Total Efficiency Visualization", px, py);
        hist->GetXaxis()->SetTitle("y [mm]");
        hist->GetYaxis()->SetTitle("z [mm]");
        c->cd();
        hist->Draw("COLZ");
        c->Update();
        c->SaveAs("plots/total_efficiency.png");
    }

    // PER-SOURCE VISUALIZATION + POINT
    if (!total_vis) {
        gROOT->SetBatch(kTRUE); // No GUI pop-ups
        const int nHists = 42;

        for (int i = 0; i < nHists; ++i) {
            TH2D *hist = (TH2D*)file->Get(Form("hist%d", i));
            if (!hist) {
                std::cerr << "Warning: 'hist" << i << "' not found in file" << std::endl;
                continue;
            }

            TCanvas *c = new TCanvas(Form("c%d", i), "Canvas", px, py);
            c->SetLeftMargin(rl);
            c->SetTopMargin(rt);
            c->SetBottomMargin(rb);
            c->SetRightMargin(rr);
            c->cd();

            hist->GetXaxis()->SetTitle("y [mm]");
            hist->GetYaxis()->SetTitle("z [mm]");
            hist->Draw("COLZ");

            // Add the source point overlay
            try {
                std::pair<double, double> pos = get_values_on_line("source_positions.txt.in", i);
                double x = pos.first;
                double y = pos.second;

                TGraph *point = new TGraph(1, &x, &y);
                point->SetMarkerStyle(20);
                point->SetMarkerSize(1.0);
                point->SetMarkerColor(kRed + 2);
                point->Draw("P SAME");
            } catch (const std::exception& e) {
                std::cerr << "Error getting source position for hist" << i << ": " << e.what() << std::endl;
            }

            c->Update();
            c->SaveAs(Form("plots/p%d.png", i));
        }
    }

    file->Close();
}


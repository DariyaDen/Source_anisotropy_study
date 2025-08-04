#include <TFile.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TH2D.h>
#include <TGraph.h>
#include <TBox.h>
#include <TLatex.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>

// Helper to read value pairs (x, y) from text file line-by-line
// Expects lines like: index x y (space-separated)
std::vector<std::pair<double, double>> read_positions_from_file(const std::string& filename) {
    std::vector<std::pair<double, double>> positions;
    std::ifstream infile(filename);
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream ss(line);
        std::string index_str;
        double x, y;

        if (!(ss >> index_str >> x >> y)) {
            // Skip lines that don't parse properly
            continue;
        }

        positions.emplace_back(x, y);
    }

    return positions;
}

void visu() {
    // === Settings ===
    const int nSources = 42;
    const int nOMs = 260;
    int nbinsx = 20, nbinsy = 13;

    double rl = 0.09, rr = 0.35, rt = 0.1, rb = 0.1;
    double px = 1000;
    double py = (px / (double(nbinsx)/nbinsy)) * (1 - rl - rr) / (1 - rb - rt);

    // === Load histograms ===
    TFile* file = TFile::Open("total_eff.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening total_eff.root\n";
        return;
    }

    // === Load source & OM positions ===
    std::vector<std::pair<double, double>> src_pos = read_positions_from_file("source_positions.txt");
    std::vector<std::pair<double, double>> om_pos  = read_positions_from_file("/sps/nemo/scratch/ddenysenko/GE/om_positions.txt");

    std::cout << "Loaded " << src_pos.size() << " sources and " << om_pos.size() << " OMs\n";

    if (src_pos.size() != nSources || om_pos.size() != nOMs) {
        std::cerr << "Mismatch in expected source or OM count!\n";
        return;
    }

    gROOT->SetBatch(kTRUE);  // suppress GUI

    for (int i = 0; i < nSources; ++i) {
        TH2D* hist = (TH2D*)file->Get(Form("hist%d", i));
        if (!hist) {
            std::cerr << "Missing histogram: hist" << i << "\n";
            continue;
        }

        TCanvas* c = new TCanvas(Form("c%d", i), Form("Source %d Visualization", i), px, py);
        c->SetLeftMargin(rl);
        c->SetTopMargin(rt);
        c->SetBottomMargin(rb);
        c->SetRightMargin(rr);
        c->cd();

        hist->GetXaxis()->SetTitle("y [mm]");
        hist->GetYaxis()->SetTitle("z [mm]");
        hist->Draw("COLZ");

        // === Plot Source i as a Green Square ===
        double x = src_pos[i].first;
        double y = src_pos[i].second;
        TGraph* src_graph = new TGraph(1, &x, &y);
        src_graph->SetMarkerStyle(20);  // square
        src_graph->SetMarkerSize(1.0);
        src_graph->SetMarkerColor(kGreen + 2);
        src_graph->Draw("P same");

        // === Plot Contributing OMs as Red Boxes ===
        std::vector<TBox*> boxes;
        TLatex latex;
        latex.SetTextSize(0.015);
        latex.SetTextAlign(12); // left aligned

        for (int j = 0; j < nOMs; ++j) {
            double content = hist->GetBinContent(j + 1); // assume each OM is assigned to one bin index
            if (content <= 0) continue;

            double x_om = om_pos[j].first;
            double y_om = om_pos[j].second;

            TBox* box = new TBox(x_om - 128, y_om - 128, x_om + 128, y_om + 128);
            box->SetLineColor(kRed);
            box->SetFillStyle(0); // transparent
            box->Draw("same");
            boxes.push_back(box);

            latex.DrawLatex(x_om + 0.3, y_om, std::to_string(j).c_str());
        }

        c->Update();
        c->SaveAs(Form("plots/source_%02d.png", i));
        delete c;
    }

    file->Close();
}


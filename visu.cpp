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

    // PER-SOURCE VISUALIZATION
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
            c->cd();
            c->SetLeftMargin(rl);
            c->SetTopMargin(rt);
            c->SetBottomMargin(rb);
            c->SetRightMargin(rr);

            hist->GetXaxis()->SetTitle("y [mm]");
            hist->GetYaxis()->SetTitle("z [mm]");

            hist->Draw("COLZ");
            c->Update();
            c->SaveAs(Form("plots/p%d.png", i));
        }
    }

    file->Close();
}

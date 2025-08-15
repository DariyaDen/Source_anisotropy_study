// File: t6_2_standalone.cpp
// Compile with:
// g++ t6_2_standalone.cpp $(root-config --cflags --libs) -o t6_2_standalone
// Run with:
// ./t6_2_standalone

#include <iostream>
#include <string>
#include <cmath>

#include "TROOT.h"
#include "TApplication.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TF1.h"
#include "TRandom.h"

using namespace std;

void t6_2(int n_toys = 10000,
          int n_tot_entries = 100,
          int nbins = 40,
          bool do_chi2 = true) {

    TString method_prefix("Log-Likelihood ");
    if (do_chi2)
        method_prefix = "#chi^{2} ";

    // Create histo
    TH1F h4(method_prefix + "h4",
            method_prefix + " Random Gauss",
            nbins, -4, 4);
    h4.SetMarkerStyle(21);
    h4.SetMarkerSize(0.8);
    h4.SetMarkerColor(kRed);

    // Histogram for sigma and t6_2
    TH1F sigma(method_prefix + "sigma",
               method_prefix + "sigma from gaus fit",
               50, 0.5, 1.5);
    TH1F t6_2_hist(method_prefix + "t6_2",
                   method_prefix + "t6_2 from gaus fit",
                   50, -4., 4.);

    // Canvases
    auto c0 = new TCanvas(method_prefix + "Gauss",
                          method_prefix + "Gauss", 0, 0, 320, 240);
    c0->SetGrid();

    auto c1 = new TCanvas(method_prefix + "Result",
                          method_prefix + "Sigma-Distribution",
                          0, 300, 600, 400);

    float sig, mean;
    for (int i = 0; i < n_toys; i++) {
        h4.Reset();

        // Fill histo with Gaussian numbers
        for (int j = 0; j < n_tot_entries; j++)
            h4.Fill(gRandom->Gaus());

        // Perform fit
        if (do_chi2)
            h4.Fit("gaus", "q"); // Chi2 fit
        else
            h4.Fit("gaus", "lq"); // Likelihood fit

        // Draw occasionally
        if (!(i % 100)) {
            h4.Draw("ep");
            c0->Update();
        }

        // Get fit results
        TF1 *fit = h4.GetFunction("gaus");
        sig = fit->GetParameter(2);
        mean = fit->GetParameter(1);

        sigma.Fill(sig);
        t6_2_hist.Fill(mean / sig * sqrt(n_tot_entries));
    }

    // Save results
    c1->cd();
    t6_2_hist.DrawClone();
    c1->SaveAs(do_chi2 ? "t6_2_chi2.png" : "t6_2_loglik.png");
    sigma.SaveAs(do_chi2 ? "sigma_chi2.root" : "sigma_loglik.root");
}

int main(int argc, char **argv) {
    TApplication app("app", &argc, argv);

    int n_toys = 10000;
    int n_tot_entries = 100;
    int n_bins = 40;

    cout << "Performing t6_2 Experiment with Chi2 method\n";
    t6_2(n_toys, n_tot_entries, n_bins, true);

    cout << "Performing t6_2 Experiment with Log Likelihood method\n";
    t6_2(n_toys, n_tot_entries, n_bins, false);

    return 0;
}


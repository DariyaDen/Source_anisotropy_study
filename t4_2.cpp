#include <TApplication.h>
#include <TCanvas.h>
#include <TGraphPolar.h>
#include <TMath.h>
#include <TROOT.h>
#include <iostream>
using namespace std;

// Original macro
void macro3() {
    auto c = new TCanvas("myCanvas", "myCanvas", 600, 600);
    double rmin = 0.;
    double rmax = TMath::Pi() * 6.;
    const int npoints = 1000;
    double r[npoints];
    double theta[npoints];
    for (int ipt = 0; ipt < npoints; ipt++) {
        theta[ipt] = ipt * (rmax - rmin) / npoints + rmin;
        r[ipt] = TMath::Sin(theta[ipt]);
    }
    auto grP1 = new TGraphPolar(npoints, theta, r);
    grP1->SetTitle("A Fan");
    grP1->SetLineWidth(3);
    grP1->SetLineColor(2);
    grP1->Draw("L");
    gPad->Update();
    grP1->GetPolargram()->SetToRadian();
}

// Wrapper for standalone execution
void StandaloneApplication(int argc, char** argv) {
    macro3(); // Call your macro
}

// Main entry point
int main(int argc, char** argv) {
    TApplication app("ROOT Application", &argc, argv);
    StandaloneApplication(app.Argc(), app.Argv());
    app.Run(); // Keeps graphics window open
    return 0;
}


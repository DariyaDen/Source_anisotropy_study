#include <TApplication.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TProfile.h> 
#include <TRandom3.h>
#include <TStyle.h>
#include <TROOT.h>
#include <iostream>
using namespace std;

void t5_3() {
    gStyle->SetPalette(kBird);
    gStyle->SetOptStat(0);
    gStyle->SetOptTitle(0);

    auto bidi_h = new TH2F("bidi_h", "2D Histo;Gaussian Vals;Exp. Vals",
                           30, -5, 5,   // X axis
                           30, 0, 10);  // Y axis

    TRandom3 rgen;
    for (int i = 0; i < 500000; i++) {
        bidi_h->Fill(rgen.Gaus(0,2), 10 - rgen.Exp(4), 0.1);
    }

    auto c = new TCanvas("Canvas","Canvas",800,800);
    c->Divide(2,2);
    c->cd(1); bidi_h->Draw("Cont1");
    c->cd(2); bidi_h->Draw("Colz");
    c->cd(3); bidi_h->Draw("Lego2");
    c->cd(4); bidi_h->Draw("Surf3");

    // Profiles and Projections
    auto c2 = new TCanvas("Canvas2","Canvas2",800,800);
    c2->Divide(2,2);
    c2->cd(1); bidi_h->ProjectionX()->Draw();
    c2->cd(2); bidi_h->ProjectionY()->Draw();
    c2->cd(3); bidi_h->ProfileX()->Draw();
    c2->cd(4); bidi_h->ProfileY()->Draw();
}

// Wrapper for standalone execution
void StandaloneApplication(int argc, char** argv){
    t5_3();
}

// Main entry point
int main(int argc, char** argv){
    TApplication app("ROOT Application", &argc, argv);
    StandaloneApplication(app.Argc(), app.Argv());
    app.Run();  // Keeps canvases interactive
    return 0;
}


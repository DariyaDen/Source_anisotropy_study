#include <TApplication.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TH1D.h>       // <-- for projections
#include <THStack.h>
#include <TF2.h>
#include <TRandom3.h>
#include <TStyle.h>

void t5_4() {
    gStyle->SetOptStat(0);

    // Define 2D functions
    TF2 *f1 = new TF2("f1","xygaus + xygaus(5) + xylandau(10)",-4,4,-4,4);
    Double_t params1[] = {130,-1.4,1.8,1.5,1, 150,2,0.5,-2,0.5, 3600,-2,0.7,-3,0.3};
    f1->SetParameters(params1);

    TF2 *f2 = new TF2("f2","xygaus + xygaus(5)",-4,4,-4,4);
    Double_t params2[] = {100,-1.4,1.9,1.1,2, 80,2,0.7,-2,0.5};
    f2->SetParameters(params2);

    // Create 2D histograms
    TH2F *h2sta = new TH2F("h2sta","Histogram 1",20,-4,4,20,-4,4);
    h2sta->SetFillColor(38);
    h2sta->FillRandom("f1",4000);

    TH2F *h2stb = new TH2F("h2stb","Histogram 2",20,-4,4,20,-4,4);
    h2stb->SetFillColor(46);
    h2stb->FillRandom("f2",3000);

    // Canvas for 2D overlay
    TCanvas *c2D = new TCanvas("c2D","Overlay 2D Histograms",800,600);
    h2sta->Draw("COLZ");
    h2stb->Draw("SAME COLZ");

    // Stack the X projections (TH1D*!)
    TH1D *proj1 = h2sta->ProjectionX("proj1");
    TH1D *proj2 = h2stb->ProjectionX("proj2");

    THStack *stack = new THStack("stack","Stacked X Projections;X axis;Counts");
    proj1->SetFillColor(38);
    proj2->SetFillColor(46);
    stack->Add(proj1);
    stack->Add(proj2);

    TCanvas *cStack = new TCanvas("cStack","Stacked Projections",800,600);
    stack->Draw();
}

// Standalone execution
int main(int argc, char** argv) {
    TApplication app("ROOT Application", &argc, argv);
    t5_4();
    app.Run(); // Keep canvases interactive
    return 0;
}


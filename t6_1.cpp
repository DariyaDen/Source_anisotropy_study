#include <TStyle.h>
#include <TF1.h>
#include <TH1F.h>
#include <TMath.h>
#include <TLatex.h>
#include <TMatrixDSym.h>
#include <TFitResult.h>
#include <TCanvas.h>
#include <iostream>

void format_line(TAttLine* line, int col, int sty) {
    line->SetLineWidth(5);
    line->SetLineColor(col);
    line->SetLineStyle(sty);
}

double the_gausppar(double* vars, double* pars) {
    return pars[0]*TMath::Gaus(vars[0],pars[1],pars[2]) +
           pars[3] + pars[4]*vars[0] + pars[5]*vars[0]*vars[0];
}

int t6_1() {
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    gStyle->SetStatBorderSize(0);
    gStyle->SetStatX(.89);
    gStyle->SetStatY(.89);

    TF1 parabola("parabola","[0]+[1]*x+[2]*x**2",0,20);
    format_line(&parabola,kBlue,2);

    TF1 gaussian("gaussian","[0]*TMath::Gaus(x,[1],[2])",0,20);
    format_line(&gaussian,kRed,2);

    TF1 gausppar("gausppar",the_gausppar,0,20,6);
    double a=15; double b=-1.2; double c=.03;
    double norm=4; double mean=7; double sigma=1;
    gausppar.SetParameters(norm,mean,sigma,a,b,c);
    gausppar.SetParNames("Norm","Mean","Sigma","a","b","c");
    format_line(&gausppar,kBlue,1);

    TH1F histo("histo","Signal plus background;X vals;Y Vals",50,0,20);
    histo.SetMarkerStyle(8);

    // Fake data
    for (int i=1; i<=5000; ++i)
        histo.Fill(gausppar.GetRandom());

    // Reset params before fitting
    gausppar.SetParameter(0,50);
    gausppar.SetParameter(1,6);
    int npar=gausppar.GetNpar();
    for (int ipar=2; ipar<npar; ++ipar)
        gausppar.SetParameter(ipar,1);

    // Perform fit
    TFitResultPtr fitResPtr = histo.Fit(&gausppar, "S");
    fitResPtr->Print();

    // Covariance matrix
    TMatrixDSym covMatrix(fitResPtr->GetCovarianceMatrix());
    covMatrix.Print();

    // Set gaussian & parabola params
    for (int ipar=0; ipar<3; ipar++) {
        gaussian.SetParameter(ipar,gausppar.GetParameter(ipar));
        parabola.SetParameter(ipar,gausppar.GetParameter(ipar+3));
    }

    // Create canvas for saving
    TCanvas c1("c1", "Fit Example", 800, 600);
    histo.GetYaxis()->SetRangeUser(0,250);
    histo.DrawClone("PE");
    parabola.DrawClone("Same");
    gaussian.DrawClone("Same");

    TLatex latex(2,220,"#splitline{Signal Peak over}{background}");
    latex.DrawClone("Same");

    // Save to PNG
    c1.SaveAs("fit_result.png");

    return 0;
}

int main(int argc, char** argv) {
    return t6_1();
}


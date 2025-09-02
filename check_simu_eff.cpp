#include <TFile.h>
#include <TH2F.h>
#include <iostream>

void check_simu_eff()
{
    TFile* f = TFile::Open("simu_eff.root","READ");
    if(!f || f->IsZombie()) {
        std::cerr << "Cannot open simu_eff.root" << std::endl;
        return;
    }

    TH2F* hEff = (TH2F*)f->Get("hEff");
    if(!hEff) {
        std::cerr << "Histogram hEff not found!" << std::endl;
        f->Close();
        return;
    }

    int N_sources = hEff->GetNbinsX();
    int N_OMs     = hEff->GetNbinsY();

    std::cout << "Histogram info: " << N_sources << " sources, " << N_OMs << " OMs" << std::endl;

    for(int i=1; i<=N_sources; ++i) {
        double sum = 0;
        for(int j=1; j<=N_OMs; ++j) {
            sum += hEff->GetBinContent(i,j);
        }
        std::cout << "Source " << i-1 << ": total efficiency sum = " << sum << std::endl;
    }

    f->Close();
}


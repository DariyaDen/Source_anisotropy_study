#include <TFile.h>
#include <TNtuple.h>
#include <TRandom3.h>
#include <iostream>

int t7_2_1() {
    TFile ofile("conductivity_experiment.root", "RECREATE");
    if (ofile.IsZombie()) {
        std::cerr << "Error: could not create file conductivity_experiment.root" << std::endl;
        return 1;
    }

    TNtuple cond_data("cond_data", "Example N-Tuple",
                      "Potential:Current:Temperature:Pressure");

    TRandom3 rndm(0);

    float pot, cur, temp, pres;
    for (int i = 0; i < 10000; ++i) {
        pot  = rndm.Uniform(0., 10.);
        temp = rndm.Uniform(250., 350.);
        pres = rndm.Uniform(0.5, 1.5);

        cur = pot / (10. + 0.05 * (temp - 300.) - 0.2 * (pres - 1.));

        pot  *= rndm.Gaus(1., 0.01);
        temp += rndm.Gaus(0., 0.3);
        pres *= rndm.Gaus(1., 0.02);
        cur  *= rndm.Gaus(1., 0.01);

        cond_data.Fill(pot, cur, temp, pres);
    }

    cond_data.Write();
    ofile.Close();

    std::cout << "Data written to conductivity_experiment.root" << std::endl;
    return 0;
}

// --- Standalone main ---
int main(int argc, char** argv) {
    return t7_2_1();
}


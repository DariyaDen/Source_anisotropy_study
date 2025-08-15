#include <TFile.h>
#include <TNtuple.h>
#include <iostream>

using namespace std;

int t7_2_2() {
    // Open the file containing the N-tuple
    TFile in_file("conductivity_experiment.root");
    if (in_file.IsZombie()) {
        cerr << "Error: could not open file conductivity_experiment.root" << endl;
        return 1;
    }

    TNtuple* my_tuple = nullptr;
    in_file.GetObject("cond_data", my_tuple);
    if (!my_tuple) {
        cerr << "Error: could not get TNtuple cond_data" << endl;
        return 2;
    }

    float pot, cur, temp, pres;

    cout << "Potential\tCurrent\tTemperature\tPressure\n";

    // Loop over entries
    for (int irow = 0; irow < my_tuple->GetEntries(); ++irow) {
        my_tuple->GetEntry(irow);
        my_tuple->SetBranchAddress("Potential", &pot);
        my_tuple->SetBranchAddress("Current", &cur);
        my_tuple->SetBranchAddress("Temperature", &temp);
        my_tuple->SetBranchAddress("Pressure", &pres);

        cout << pot << "\t" << cur << "\t" << temp << "\t" << pres << endl;
    }

    return 0;
}

// --- Standalone main ---
int main(int argc, char** argv) {
    return t7_2_2();
}


#include <TApplication.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TROOT.h>
#include <iostream>
using namespace std;

// Original macro function
int t4_1() {
    auto c = new TCanvas();
    c->SetGrid();

    // Expected points (yellow band)
    TGraphErrors graph_expected("./t4_1_input_expected.txt", "%lg %lg %lg");
    graph_expected.SetTitle("Measurement XYZ and Expectation;length [cm];Arb.Units");
    graph_expected.SetFillColor(kYellow);
    graph_expected.DrawClone("E3AL"); // E3 draws error band

    // Measured points
    TGraphErrors graph("t4_1_input_expected.txt", "%lg %lg %lg");
    graph.SetMarkerStyle(kCircle);
    graph.SetFillColor(0);
    graph.DrawClone("PESame");

    // Legend
    TLegend leg(.1, .7, .3, .9, "Lab. Lesson 2");
    leg.SetFillColor(0);
    leg.AddEntry(&graph_expected, "Expected Points");
    leg.AddEntry(&graph, "Measured Points");
    leg.DrawClone("Same");

    graph.Print();
    return 0;
}

// Wrapper for standalone execution
void StandaloneApplication(int argc, char** argv) {
    t4_1(); // Call your macro
}

// Main entry point for standalone ROOT app
int main(int argc, char** argv) {
    TApplication app("ROOT Application", &argc, argv);
    StandaloneApplication(app.Argc(), app.Argv());
    app.Run(); // Keeps graphics window open
    return 0;
}


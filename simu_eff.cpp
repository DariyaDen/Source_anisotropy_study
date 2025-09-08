#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TH2D.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <array>
#include <unordered_map>

#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiEvent.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiCD.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiCDCaloHit.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiGID.h"

R__LOAD_LIBRARY(/sps/nemo/scratch/ddenysenko/GE/MiModule/lib/libMiModule.so);

const int N_sources = 42;
const int N_OMs = 260;

const int NBX = 20;
const int NBY = 13;
const double SY = 256.0;
const double SZ = 256.0;
const double XH = NBX/2.0 * SY;
const double YH = NBY/2.0 * SZ;

int safe_stoi(const std::string& s, int def=-1) {
    try {
        if (s.empty()) {
    return def;
} else {
    return std::stoi(s);
}

    } catch (...) {
        return def;
    }
}

// ----------------- OM mapping -----------------
int SWCR_to_OMnum(const std::string &type, int side, int wall, int column, int row) {
    if (type == "1302") return (side==0) ? column*13 + row : 260 + column*13 + row;
    if (type == "1232") return (side==0) ? 520 + wall*32 + column*16 + row : 584 + wall*32 + column*16 + row;
    if (type == "1252") return (side==0) ? 648 + wall*16 + row : 680 + wall*16 + row;
    return -1;
}

// Precompute source positions
std::vector<std::pair<double,double>> read_sources(const std::string &filename, int NH) 
{
    std::ifstream infile(filename);
    std::vector<std::pair<double,double>> sources(NH, {0.0,0.0});
    if (!infile) throw std::runtime_error("Cannot open " + filename);
    std::string line;
    for(int i=0; i<NH && std::getline(infile,line); ++i) {
        std::replace(line.begin(), line.end(), ';', ' ');
        std::istringstream ss(line);
        double x, y; ss >> x >> y;
        sources[i] = {x, y};
    }
    return sources;
}

// Precompute OM positions
std::array<double,3> OMnum_to_position(int OM_num)
{
    array<double,4> SWCR;
    array<double,3> xyz;
    //mainwall IT
	if(OM_num < 260) 
	{
		SWCR[0] = 0;
		SWCR[1] = -1;
		SWCR[2] = OM_num / 13;
		SWCR[3] = OM_num % 13;
	}
	//mainwall FR
	else if(OM_num < 520)
	{
		SWCR[0] = 1;
		SWCR[1] = -1;
		SWCR[2] = (OM_num - 260) / 13;
		SWCR[3] = (OM_num - 260) % 13;
	}
	//Xcalo IT	
	else if(OM_num < 584)
	{
		SWCR[0] = 0;
		SWCR[1] = (OM_num - 520) / 32;
		SWCR[2] = ((OM_num - 520) / 16) % 2;
		SWCR[3] = (OM_num -520) % 16;
	}
	//Xcalo FR
	else if(OM_num < 648)
	{
		SWCR[0] = 1;
		SWCR[1] = (OM_num - 520 - 64) / 32;
		SWCR[2] = ((OM_num - 520 - 64) / 16) % 2;
		SWCR[3] = (OM_num -520 - 64) % 16;
	}
	//GVeto IT
	else if(OM_num < 680)
	{
		SWCR[0] = 0;
		SWCR[1] = (OM_num - 520 - 128) / 16;
		SWCR[2] = (OM_num - 520 - 128) % 16;
		SWCR[3] = -1;
	}
	//GVeto FR
	else if(OM_num < 712)
	{
		SWCR[0] = 1;
		SWCR[1] = (OM_num - 520 - 128 - 32) / 16;
		SWCR[2] = (OM_num - 520 - 128 - 32) % 16;
		SWCR[3] = -1;
	}
	
	int OM_type;
	
	if(OM_num < 520)
	{
		OM_type = 1302;
	}
	else if(OM_num < 648)
	{
		OM_type = 1232;
	}
	else
	{
		OM_type = 1252;
	}

	switch(OM_type)
	{
		case 1302: //MW
			if(SWCR[0] == 1)
				xyz[0] = 532.0;
			else
				xyz[0] = -532.0;
				xyz[1] = ((double)SWCR[2]- 9.5) * 259.0;
				xyz[2] = ((double)SWCR[3] - 6) * 259.0;
				
			break;
			
		case 1232: //XW
			if(SWCR[1] == 1)
				xyz[1] = 2580.5;
			else
				xyz[1] = -2580.5;
				
				if(SWCR[0] == 1)
			{
				if(SWCR[2] == 1)
					xyz[0] = 333.0;
				else
					xyz[0] = 130.0;
			}
			else
			{
				if(SWCR[2] == 1)
					xyz[0] = -333.0;

				else
					xyz[0] = -130.0;
			}
			
			xyz[2] = ((double)SWCR[3] - 7.5) * 212.0;
			
			break;
			
		case 1252: //GV
			if(SWCR[0] == 1)
				xyz[0] = 213.5;
			else
				xyz[0] = -213.5;
			if(SWCR[1] == 1)
				xyz[2] = 1625.0;
			else
				xyz[2] = -1625.0;
			if(SWCR[2] > 7)
				xyz[1] = 161.0 + (((double)SWCR[2]-8) * 311.5);
			else
				xyz[1] = -161.0 + (((double)SWCR[2]-7) * 311.5);
			break;	

	}
    return xyz;
}

// ----------------- count before -----------------
void count_root_before(const std::string &rootfile, int N_before[N_sources]) {
    std::fill(N_before, N_before+N_sources, 0);

    TFile* f = TFile::Open(rootfile.c_str(),"READ");
    if(!f || f->IsZombie()) return;

    TTreeReader reader("Event", f);
    while(reader.Next()) {
        static int ie = 0;
        int srcID = ie % N_sources;
        ++N_before[srcID];
        ++ie;
    }
    f->Close();
}

// ----------------- count after -----------------
void count_root_after(const std::string &rootfile, int N_after[N_sources][N_OMs]) {
    for(int i=0;i<N_sources;++i)
        std::fill(N_after[i], N_after[i]+N_OMs, 0);

    TFile* f = TFile::Open(rootfile.c_str(),"READ");
    if(!f || f->IsZombie()) return;

    TTreeReader reader("Event", f);
    TTreeReaderValue<MiEvent> event(reader, "Eventdata");

    long long ie = 0;
    while(reader.Next()) {
        MiEvent* Eve = &*event;
        int srcID = ie % N_sources;
        ++ie;

        MiCD* cd = Eve->getCD();
        if(!cd) continue;
        int nCalo = cd->getnoofcaloh();

        for(int ih=0; ih<nCalo; ++ih){
    MiCDCaloHit* hit = cd->getcalohit(ih);
    if(!hit) continue;
    MiGID* gid = hit->getGID();
    if(!gid) continue;

    std::string type = gid->gettype();
    int side   = safe_stoi(gid->getside());
    int wall   = safe_stoi(gid->getwall());
    int column = safe_stoi(gid->getcolumn());
    int row    = safe_stoi(gid->getrow());

    // Debug print only, don’t skip
    if (side < 0 || wall < -1 || column < 0 || row < -2) {
        std::cerr << "⚠️ Strange GID at event " << ie
                  << " srcID=" << srcID
                  << " type=" << type
                  << " side='" << gid->getside() << "'"
                  << " wall='" << gid->getwall() << "'"
                  << " col='" << gid->getcolumn() << "'"
                  << " row='" << gid->getrow() << "'"
                  << std::endl;
    }

    // Always try mapping
            int omID = SWCR_to_OMnum(type, side, wall, column, row);
    if(omID >= 0 && omID < N_OMs) {
        N_after[srcID][omID]++;
    }
}


    }

    f->Close();
}

// ----------------- main macro -----------------
void simu_eff(int startDir, int endDir){
    int N_before[N_sources] = {0};
    int N_after[N_sources][N_OMs] = {{0}};

    for(int i=startDir; i<=endDir; ++i){
        std::cout << "Processing directory " << i << std::endl;
        count_root_before("DATA/"+std::to_string(i)+"/0cut/Default.root", N_before);
        count_root_after("DATA/"+std::to_string(i)+"/Default.root", N_after);
    }

    // Precompute positions
    std::vector<std::array<double,3>> OM_positions(N_OMs);
    for(int j=0;j<N_OMs;j++) OM_positions[j] = OMnum_to_position(j);

    auto source_positions = read_sources("source_positions.txt.in", N_sources);

    // Write efficiency histograms
    TFile* fout = new TFile("simu_eff.root","RECREATE");
    
    
    for (int k=0; k<N_sources; k++) {
    TH2D* hist = new TH2D(Form("hist%d",k),
                          Form("Efficiency source %d",k),
                          NBX, -XH, XH,   // X axis: -2560 .. +2560
                          NBY, -YH, YH);  // Y axis: -1664 .. +1664

    for (int j=0; j<N_OMs; j++) {
    double eff;
if (N_before[k] > 0) {
    eff = double(N_after[k][j]) / N_before[k];
} else {
    eff = 0.0;
    std::cout<<"Here is a mistake";
}
        // Use detector coordinates
        double x = OM_positions[j][1];  // Y coordinate of OM
        double y = OM_positions[j][2];  // Z coordinate of OM

        // Fill histogram with efficiency weight
        hist->Fill(x, y, eff);
    }

    hist->Write();
}

 /*  for(int k=0;k<N_sources;k++){
        TH2D* hist = new TH2D(Form("hist%d",k),
                              Form("Efficiency source %d",k),
                              NBX, -XH, XH, NBY, -YH, YH);

      //  for(int i=0;i<N_sources;i++){
      for (int j=0; j<N_OMs; j++) {
    int xbin = j / NBY;
    int ybin = j % NBY;
    double eff = (N_before[k] > 0) ? double(N_after[k][j]) / N_before[k] : 0.0;
    std::cout << N_before[k] << " " << eff << std::endl;
    if (xbin < NBX && ybin < NBY) {
        hist->SetBinContent(xbin+1, ybin+1, eff);
    }
}
/*
        for(int j=0;j<N_OMs;j++){
            int ybin=j/NBY;
            int zbin=j%NBY;
            double eff = (N_before[k]>0) ? double(N_after[k][j])/N_before[k] : 0.0;
            std::cout<<N_before[k]<<" "<<eff<<endl;
            hist->SetBinContent(ybin+1, zbin+1, eff);
        } */ // }
    //    hist->Write();
  //  } 
    fout->Close();
}


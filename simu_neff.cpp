#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <cmath>

#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiEvent.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiCD.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiCDCaloHit.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiGID.h"

R__LOAD_LIBRARY(/sps/nemo/scratch/ddenysenko/GE/MiModule/lib/libMiModule.so);

const int N_sources = 42;
const int N_OMs = 712;

std::array<double,3> OMnum_to_position(int OM_num);

std::array<double,3> OMnum_to_position(int OM_num); 

// ------------------- load OMmap from positions ------------------- 

std::map<std::pair<int,int>, int> loadOMmap(int N_OMs) 
{ std::map<std::pair<int,int>, int> OMmap; 
for(int om=0; om<N_OMs; ++om) 
{ auto pos = OMnum_to_position(om); OMmap[{int(std::round(pos[1])), int(std::round(pos[2]))}] = om; 
// use y,z 
} return OMmap; 
}

// ------------------- count before-cuts events -------------------

void count_root_before(const std::string& rootfile, int N_before[N_sources])
{
    // Initialize counts
    for (int i = 0; i < N_sources; ++i) N_before[i] = 0;

    TFile* f = TFile::Open(rootfile.c_str(), "READ");
    if (!f || f->IsZombie()) {
        std::cerr << "Cannot open " << rootfile << std::endl;
        return;
    }

    TTree* t = f->Get<TTree>("SimData");
    if (!t) {
        std::cerr << "No tree SimData in " << rootfile << std::endl;
        f->Close();
        return;
    }

    Long64_t nEntries = t->GetEntries();

    // Loop through events
    for (Long64_t ie = 0; ie < nEntries; ++ie) {
        t->GetEntry(ie);

        // simple mapping: assign events to sources in order
        int srcID = ie % N_sources;
        N_before[srcID]++;
    }

    f->Close();
}

// ------------------- count fired OMs -------------------
void count_root_after(const std::string& rootfile,
                      int N_after[N_sources][N_OMs],
                      const std::map<std::pair<int,int>, int>& OMmap)
{
    // Initialize counts
    for (int i = 0; i < N_sources; ++i)
        for (int j = 0; j < N_OMs; ++j)
            N_after[i][j] = 0;

    TFile* f = TFile::Open(rootfile.c_str(),"READ");
    if(!f || f->IsZombie()) { 
        std::cerr<<"Cannot open "<<rootfile<<std::endl; 
        return; 
    }

    TTree* t = f->Get<TTree>("Event");
    if(!t){ 
        std::cerr<<"No tree Event in "<<rootfile<<std::endl; 
        f->Close(); 
        return; 
    }

    MiEvent* Eve = nullptr;
    t->SetBranchAddress("Eventdata",&Eve);

    Long64_t nEntries = t->GetEntries();
    for(Long64_t ie=0; ie<nEntries; ++ie){
        t->GetEntry(ie);
        int srcID = ie % N_sources;

        MiCD* cd = Eve->getCD();
        if(!cd) continue;
        int nCalo = cd->getnoofcaloh();
        for(int ih=0; ih<nCalo; ++ih){
            MiCDCaloHit* hit = cd->getcalohit(ih);
            if(!hit) continue;
            MiGID* gid = hit->getGID();
            if(!gid) continue;

            int y = static_cast<int>(std::round(std::stod(gid->getcolumn())));
            int z = static_cast<int>(std::round(std::stod(gid->getrow())));
            auto it = OMmap.find({y,z});
            if(it != OMmap.end()){
                int omID = it->second;
                if(omID >= 0 && omID < N_OMs) 
                    N_after[srcID][omID]++;
            }
        }
    }
    f->Close();
}

// ------------------- main macro -------------------
void simu_neff(int startDir, int endDir)
{
    int N_before[N_sources];
for(int i = 0; i < N_sources; ++i) N_before[i] = 0;

int N_after[N_sources][N_OMs];
for(int i = 0; i < N_sources; ++i)
    for(int j = 0; j < N_OMs; ++j)
        N_after[i][j] = 0;

auto OMmap = loadOMmap(N_OMs);

    for(int i=startDir; i<=endDir; ++i){
        std::cout << "Iteration " << i << std::endl;

        std::string rootBefore = "DATA/" + std::to_string(i) + "/Default0cuts.root";
        count_root_before(rootBefore, N_before);

        std::string rootAfter = "DATA/" + std::to_string(i) + "/Default.root";
        count_root_after(rootAfter, N_after, OMmap);
    }

    // Build efficiency histogram
    TFile* fout = new TFile("simu_eff.root", "RECREATE");
    TH2F* hEff = new TH2F("hEff", "Efficiency;Source;OM",
                          N_sources, 0, N_sources,
                          N_OMs, 0, N_OMs);

    for(int i=0; i<N_sources; ++i){
        for(int j=0; j<N_OMs; ++j){
            double eff = 0.0;
            if(N_before[i] > 0)
                eff = static_cast<double>(N_after[i][j]) / N_before[i];
               std::cout<<static_cast<double>(N_after[i][j]) <<" "<<N_before[i]<<std::endl;
            hEff->SetBinContent(i+1, j+1, eff);
        }
    }

    hEff->Write();
    fout->Close();
    std::cout << "Efficiencies written to simu_eff.root" << std::endl;
}


//turns OM_number to OM positions
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

#include <TParameter.h>
#include <TKey.h>
#include <TFile.h>
#include <TTree.h>
#include <TH2F.h>
#include <TSystem.h>
#include <TROOT.h>
#include <array>
#include <vector>
#include <string>
#include <iostream>

#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiEvent.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiVertex.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiVector3D.h"
#include "/sps/nemo/scratch/ddenysenko/GE/MiModule/include/MiPTD.h"

R__LOAD_LIBRARY(/sps/nemo/scratch/ddenysenko/GE/MiModule/lib/libMiModule.so);

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

void simu_eff(int startDir, int endDir)
{
    const int N_OMs = 712;
    const int N_sources = 42;
    Long64_t totalEntries = 0;

    // Output TTrees
    TFile* f_out = new TFile("simu_eff.root","RECREATE");

    // Ensure TTrees are created in the file's directory
    f_out->cd();

    // Event -> Source -> Vertex
    TTree* t_source = new TTree("EventSource","Event with Source and Vertex info");
    int eventID, sourceID;
    double vertX, vertY, vertZ;
    t_source->Branch("eventID",&eventID,"eventID/I");
    t_source->Branch("sourceID",&sourceID,"sourceID/I");
    t_source->Branch("vertX",&vertX,"vertX/D");
    t_source->Branch("vertY",&vertY,"vertY/D");
    t_source->Branch("vertZ",&vertZ,"vertZ/D");

    // Event -> OM -> Vertex
    TTree* t_OM = new TTree("EventOM","Event with OM and Vertex info");
    int OM_ID;
    double OMX, OMY, OMZ;
    t_OM->Branch("eventID",&eventID,"eventID/I");
    t_OM->Branch("OM_ID",&OM_ID,"OM_ID/I");
    t_OM->Branch("vertX",&vertX,"vertX/D");
    t_OM->Branch("vertY",&vertY,"vertY/D");
    t_OM->Branch("vertZ",&vertZ,"vertZ/D");
    t_OM->Branch("OMX",&OMX,"OMX/D");
    t_OM->Branch("OMY",&OMY,"OMY/D");
    t_OM->Branch("OMZ",&OMZ,"OMZ/D");

    for (int i=startDir; i<=endDir; ++i)
    {
        std::string fname = "DATA/" + std::to_string(i) + "/Default.root";
        std::cout << "Opening: " << fname << std::endl;

        TFile* f = TFile::Open(fname.c_str(),"READ");
        if(!f || f->IsZombie()) { std::cerr<<"Cannot open "<<fname<<std::endl; continue; }

        TTree* t = (TTree*)f->Get("Event");
        if(!t) { std::cerr<<"No tree named Event in "<<fname<<std::endl; f->Close(); continue; }

        MiEvent* Eve = new MiEvent();
        t->SetBranchAddress("Eventdata",&Eve);

        Long64_t nEntries = t->GetEntries();
        std::cout<<"Events in this file: "<<nEntries<<std::endl;
        totalEntries += nEntries;

        for(Long64_t ie=0; ie<nEntries; ie++)
        {
            t->GetEntry(ie);
            eventID = ie;

            // Event -> Source -> Vertex
            for(int iSource=0;iSource<N_sources;iSource++)
            {
                int nPart = Eve->getPTDNoPart();
                for(int ip=0; ip<nPart; ip++)
                {
                    int nVert = Eve->getPTDNoVert(ip);
                    for(int iv=0; iv<nVert; iv++)
                    {
                        vertX = Eve->getPTDverX(ip,iv);
                        vertY = Eve->getPTDverY(ip,iv);
                        vertZ = Eve->getPTDverZ(ip,iv);
                        sourceID = iSource;
                        t_source->Fill();
                    }
                }
            }

            // Event -> OM -> Vertex
            for(int jOM=0;jOM<N_OMs;jOM++)
            {
                std::array<double,3> OMpos = OMnum_to_position(jOM);
                OMX = OMpos[0]; OMY = OMpos[1]; OMZ = OMpos[2];
                OM_ID = jOM;

                int nPart = Eve->getPTDNoPart();
                for(int ip=0; ip<nPart; ip++)
                {
                    int nVert = Eve->getPTDNoVert(ip);
                    for(int iv=0; iv<nVert; iv++)
                    {
                        vertX = Eve->getPTDverX(ip,iv);
                        vertY = Eve->getPTDverY(ip,iv);
                        vertZ = Eve->getPTDverZ(ip,iv);
                        t_OM->Fill();
                    }
                }
            }
        }

        f->Close();
    }

    std::cout<<"Total events processed: "<<totalEntries<<std::endl;

    // Write trees to output file
    f_out->cd();
    t_source->Write();
    t_OM->Write();
    f_out->Close();
}


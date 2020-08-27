#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <iostream>

using namespace std;


struct VarRange{
	const char* name;
	Float_t lower;
	Float_t upper;
	VarRange(const char* n, Float_t l, Float_t u){name = n; lower = l; upper = u;}
};


VarRange settings[] = {VarRange("mm_kin_sl3d[bkmm_mm_index]", 0, 20),
			VarRange("Muon_eta[mm_mu1_index[bkmm_mm_index]]",-2, 2),
			VarRange("Muon_eta[mm_mu2_index[bkmm_mm_index]]",-2, 2),
			VarRange("Muon_pt[mm_mu1_index[bkmm_mm_index]]", 2, 20),
			VarRange("Muon_pt[mm_mu2_index[bkmm_mm_index]]", 2, 12),
			VarRange("bkmm_kaon_pt", 1, 6),
			VarRange("mm_kin_vtx_chi2dof[bkmm_mm_index]", 0, 20),
			VarRange("mm_kin_l3d[bkmm_mm_index]", 0, 2),
			VarRange("bkmm_bmm_iso", 0, 2),
			VarRange("mm_kin_mass[bkmm_mm_index]", 2.5 , 3.5),
			VarRange("mm_mu2_index[bkmm_mm_index]", 0, 5),
			VarRange("bkmm_kaon_charge", -3, 3),
			VarRange("bkmm_kaon_eta", -5, 5),
			VarRange("bkmm_nomc_sl3d", 0, 20),
			VarRange("bkmm_jpsimc_pt", 0, 100),
			VarRange("bkmm_jpsimc_kaon1eta", -2, 2),
			VarRange("bkmm_bmm_iso", 0, 2),
			VarRange("bkmm_nomc_l3d", 0, 0.5),
			VarRange("log(bkmm_jpsimc_vtx_chi2dof)", -10, 6),
			VarRange("bkmm_jpsimc_alpha", 0, 3.15),
			VarRange("bkmm_bmm_bdt", -0.6, 0.7)};
//const int index=2;
void example(){
	const int index = 20;
	// open the file
	TFile *f = TFile::Open("/mnt/hadoop/scratch/dmytro/NanoAOD/505/Charmonium+Run2016D-17Jul2018-v1+MINIAOD/B000929A-C68A-E811-AF3D-0CC47A7C3408.root");
	// get the tree from the file
	TTree *t = (TTree*)f->Get("Events");
	// int i = index;
	for (int i = 0; i<21; i++){
	TCanvas *c = new TCanvas();
	VarRange vardata = settings[i];
	//VarRange vardata = settings[index];	
	TH1F* in_peak  = new TH1F("in_peak" , vardata.name, 30, vardata.lower, vardata.upper);
	TH1F* off_peak = new TH1F("off_peak", vardata.name, 30, vardata.lower, vardata.upper);
	t -> Draw(Form("%s >> in_peak", vardata.name), "5.24<bkmm_jpsimc_mass && bkmm_jpsimc_mass<5.31");
	t -> Draw(Form("%s >> off_peak", vardata.name), "abs(bkmm_jpsimc_mass - 5.4) < 0.5 && (5.24>bkmm_jpsimc_mass || bkmm_jpsimc_mass>5.31)"); 
	off_peak -> Scale(1. / off_peak -> GetEntries());
	off_peak -> Draw("hist");
	in_peak  -> Scale(1. / in_peak  -> GetEntries());
	in_peak -> SetLineColor(kBlue);
	in_peak -> Draw("hist SAME");
	c -> SaveAs(Form("mass_cut_compare_%s.png", vardata.name));
	}
	// TH2F* h2 = new TH2F("h2", Form("bkmm_jpsimc_mass vs. %s", vardata.name), 20, 4.9, 5.8, 20, vardata.lower, vardata.upper);
        // t->Draw(Form("%s:bkmm_jpsimc_mass >> h2", vardata.name),"abs(bkmm_jpsimc_mass-5.4)<0.5"); 
	// "abs(bkmm_jpsimc_mass-5.4)<0.5&&mm_kin_sl3d>1.5&&abs(Muon_eta[mm_mu1_index[bkmm_mm_index]])<1.4");
	// h2 -> SetXTitle("mass");
	// h2 -> SetYTitle("Muon_eta");
	// h2 -> Print();
	// h2 -> Draw("BOX");
        // return;
	/*
	// define a cut
	string cut = "abs(bkmm_jpsimc_mass-5.4)<0.5&&mm_kin_sl3d>%f";

	// let's run this cut from 0 to 5 in steps of 0.5
	int i = 0;
	for (float cut_value=2; cut_value<5; cut_value+=0.5){

		TH1F h ("histo", "my histogram", 100, 4.9,5.6);//histogram with 100 bins between 4.9 and 5.6
		auto this_cut = Form(cut.c_str(),cut_value);
		cout<< "Drawing with cut: "<<this_cut<<endl;
		//create a canvas
		TCanvas c; //
		// draw something, using this cut
		t->Draw("bkmm_jpsimc_mass>>histo", this_cut);
		cout<< "This histogram contains "<<h.GetEntries()<<" events."<<endl;
		//save the plot as a png image
		c.SaveAs(Form("mass_%d.png",i));
		h.Reset(); //empty the histogram
		i++;
	}*/
}

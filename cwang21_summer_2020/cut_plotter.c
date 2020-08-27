#include <iostream>
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLegend.h"

#include "varrange.h"
#include "MitRootStyle/MitRootStyle.C"

const char* filename = "exampledata_3.root";
const char* tree_name = "Events";

void cut_plotter(){
	TTree* T = (TTree*) TFile::Open(filename) -> Get(tree_name);
	TH1F* h0 = new TH1F("h0", "mass", 100, 4.9, 5.8);
	TH1F* h1 = new TH1F("h1", "mass", 100, 4.9, 5.8);
	TH1F* h2 = new TH1F("h2", "mass", 100, 4.9, 5.8);
	TH1F* h3 = new TH1F("h3", "mass", 100, 4.9, 5.8);
	TH1F* h4 = new TH1F("h4", "mass", 100, 4.9, 5.8);
	TH1F* h5 = new TH1F("h5", "mass", 100, 4.9, 5.8);
	TH1F* h6 = new TH1F("h6", "mass", 100, 4.9, 5.8);
	
	const char* c1 = "bkmm_jpsimc_alpha<0.065";
	const char* c2 = "bkmm_jpsimc_alpha<0.125 && bkmm_kaon_pt>1.76";
	const char* c3 = "bkmm_jpsimc_alpha<0.125 && bkmm_kaon_pt>1.72 && bkmm_nomc_sl3d>1.92";
	const char* c4 = "bkmm_jpsimc_alpha<0.125 && bkmm_kaon_pt>1.16 && bkmm_nomc_sl3d>2.34 && log(bkmm_jpsimc_vtx_chi2dof)<1.99";
	const char* c5 = "bkmm_jpsimc_alpha<0.135 && bkmm_kaon_pt>1.16 && bkmm_nomc_sl3d>2.34 && log(bkmm_jpsimc_vtx_chi2dof)<1.99 && bkmm_bmm_bdt>-0.31";
	const char* c6 = "bkmm_bmm_bdt>0.09 && bkmm_kaon_pt>1.76";
	
	MitRootStyle::Init(-1);	
	TCanvas* c = new TCanvas();
	// T -> Print();	
	T -> Draw("bkmm_jpsimc_mass >> h0");
	T -> Draw("bkmm_jpsimc_mass >> h1", c1);
	T -> Draw("bkmm_jpsimc_mass >> h2", c2);
	T -> Draw("bkmm_jpsimc_mass >> h3", c3);
	T -> Draw("bkmm_jpsimc_mass >> h4", c4);
	T -> Draw("bkmm_jpsimc_mass >> h5", c5);	
	T -> Draw("bkmm_jpsimc_mass >> h6", c6);
	
	TH1F* frame = new TH1F("Frame", "", 1, 4.9, 5.8);
	MitRootStyle::InitHist(frame, "Event Mass [GeV]", "Counts");
	frame -> GetYaxis() -> SetRangeUser(0, 1100);
	frame -> Draw();
	
	// h0 -> Print();	
	// TCanvas* c = new TCanvas();
	MitRootStyle::InitHist(h0, "", "", kViolet);
	MitRootStyle::InitHist(h1, "", "", kBlack);
	MitRootStyle::InitHist(h2, "", "", kRed);
	MitRootStyle::InitHist(h3, "", "", kGreen);
	MitRootStyle::InitHist(h4, "", "", kBlue);
	// MitRootStyle::InitHist(h5, "", "", kBlack);
	// h1-> Print();
	// h4 -> SetLineColor(1);
	h0 -> Draw("SAME");
	h1 -> Draw("SAME");
	h2 -> Draw("SAME");
	h3 -> Draw("SAME");
	h4 -> Draw("SAME");
	// h5 -> SetLineColor(2);
	// h5 -> Draw("SAME");
	auto legend = new TLegend(.85, 0.75, .99, .99);
	legend -> AddEntry(h0, "No Cuts");
	legend -> AddEntry(h1, "1 Cut");
	legend -> AddEntry(h2, "2 Cuts");
	legend -> AddEntry(h3, "3 Cuts");
	legend -> AddEntry(h4, "4 Cuts");
	legend -> Draw();
} 

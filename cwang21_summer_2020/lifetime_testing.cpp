#include <iostream>
#include "TF1.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TPad.h"
#include "filenames.h"
#include "TLegend.h"
#include "TGraphErrors.h"

#include "MitRootStyle/MitRootStyle.C"
using namespace ROOT;
using namespace std;

struct TauError{Float_t tau; Float_t error;};
typedef struct TauError te;

te GetTau(TH1F* h, Float_t start){
        TF1* f = new TF1("f1", "[0] * exp(-x / [1])", start, 10);
        f -> SetParameter(1, 1.5);
	f -> SetParameter(0, 7e4);
        h -> Fit(f, "QN", "", start, 10);
        TF1* fit = h -> GetFunction("f1");
        Double_t params[2];
        // Double_t errors[2];
        f -> GetParameters(params);
        const Double_t* errors = f -> GetParErrors();
        te ret; 
        ret.tau = params[1];  
        ret.error = errors[1];
        return ret;
}


const char* filename = names::sim_1;
const char* tree_name = "Candidates";

const char* filename_2 = names::example_3;
const char* tree_name_2 = "Events";
void lifetime_testing(){
	MitRootStyle::Init(-1);
	TTree *T = (TTree*) TFile::Open(filename) -> Get(tree_name);
	// TTree *T2 = (TTree*) TFile::Open(filename_2) -> Get(tree_name_2);
	// TChain *C = new TChain("Events");
	// C -> Add(names::events_dir);
	// C -> Print();
	// C -> Draw("bkmm_jpsimc_mass", "abs(bkmm_jpsimc_mass - 5.4) < 0.5");
	// TChain *T = new TChain("Events");
	// T -> Add(filename);
	const char* base_cut = "0 < %s && %s < 1e-11";		
	const char* gen = "bkmm_gen_tau";
	const char* kin = "bkmm_jpsimc_tau";
	const char* gen_cut = "0 < bkmm_gen_tau && bkmm_gen_tau < 1e-11";
	const char* kin_cut = "0 < bkmm_jpsimc_tau && bkmm_jpsimc_tau < 1e-11";
	const char* c1 = "bkmm_jpsimc_alpha<0.065";
        const char* c2 = "bkmm_jpsimc_alpha<0.125 && bkmm_kaon_pt>1.76";
        const char* c3 = "bkmm_jpsimc_alpha<0.125 && bkmm_kaon_pt>1.72 && bkmm_nomc_sl3d>1.92";
        const char* c4 = "bkmm_jpsimc_alpha<0.125 && bkmm_kaon_pt>1.16 && bkmm_nomc_sl3d>2.34 && log(bkmm_jpsimc_vtx_chi2dof)<1.99";
        const char* c5 = "bkmm_jpsimc_alpha<0.135 && bkmm_kaon_pt>1.16 && bkmm_nomc_sl3d>2.34 && log(bkmm_jpsimc_vtx_chi2dof)<1.99 && bkmm_bmm_bdt>-0.31";
        const char* c6 = "bkmm_bmm_bdt>0.09 && bkmm_kaon_pt>1.76";

	TH1F* h0 = new TH1F("h0", "tau", 100, 0, 10);
	TH1F* h1 = new TH1F("h1", "tau", 100, 0, 10);
	TH1F* h2 = new TH1F("h2", "tau", 100, 0, 10);
	/* MitRootStyle::InitHist(h0, "", "");
	MitRootStyle::InitHist(h1, "", "");
	MitRootStyle::InitHist(h2, "", "");*/
	// TCanvas* c = new TCanvas();
	// c -> SetLogY();
	T -> Draw("1e12 * bkmm_gen_tau >> h0", gen_cut);
	T -> Draw("1e12 * bkmm_jpsimc_tau >> h1", kin_cut);
	T -> Draw("1e12 * bkmm_jpsimc_tau >> h2", Form("%s && %s", kin_cut, c4));
	// C -> Draw("bkmm_jpsimc_tau >> h1", Form("%s && %s && abs(bkmm_jpsimc_mass - 5.27) < 0.3", kin_cut, c4));
	// C -> Draw("bkmm_jpsimc_tau >> h2", Form("%s && %s && ((bkmm_jpsimc_mass > 4.9 && bkmm_jspimc_mass < 5.05) || (bkmm_jpsimc_mass > 5.5 && bkmm_jpimc_mass < 5.8))", kin_cut, c4));
	// C -> Draw("bkmm_jpsimc_tau: bkmm_jpsimc_mass", Form("abs(bkmm_jpsimc_mass - 5.4) < 0.5 && %s && %s", kin_cut, c4), "LEGO");
	// gPad -> SetLogy();	
	h0 -> SetLineColor(1);
	h1 -> SetLineColor(2);
	h2 -> SetLineColor(3);
	// TCanvas* c = new TCanvas();
	// gPad -> SetLogy();	
	// MitRootStyle::Init(-1);
	// TH1F *frame = new TH1F("Frame", "", 1, 0, 10);
	// MitRootStyle::InitHist(frame, "Lifetime [[s]", "Counts");
	/* frame -> SetMinimum(1e2);
	frame -> SetMaximum(2e5);
	frame -> Draw();
	h0 -> Draw("SAME");
	h1 -> Draw("SAME");
	h2 -> Draw("SAME");
	// h1 -> DrawNormalized();// "SAME");
	// h2 -> DrawNormalized("SAME");
	TLegend* legend = new TLegend(0.51, .7, .9, .9);
	legend -> AddEntry(h0, "Generated");
	legend -> AddEntry(h1, "Reconstructed");
	legend -> AddEntry(h2, "Reconstructed, cut");
	legend -> Draw();*/
	
	const int N = 60;
	int offset = 2;
	Float_t x[N], y[N], ex[N], ey[N];	
	for (int i = 0; i < N; i += 1){
		Float_t start = (i + offset) * .1;
		te res = GetTau(h2, start);
		cout << res.tau << res.error << endl;	
		
		x[i] = start;
		y[i] = res.tau;
		ex[i] = 0;
		ey[i] = res.error;
		
	}
	auto gr = new TGraphErrors(N, x, y, ex, ey);
	MitRootStyle::InitGraph(gr, "Lifetime cut [ps]", "Fitted lifetime [ps]");
	gPad -> SetLeftMargin(.20);
	gr -> Draw();
}

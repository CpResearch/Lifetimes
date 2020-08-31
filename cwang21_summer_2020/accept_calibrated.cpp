#include <iostream>
#include "TF1.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TPad.h"
#include "TGraphErrors.h"

#include "filenames.h"
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


const char* filename = names::sim_3;
const char* tree_name = "Events";

void accept_calibrated(){
	MitRootStyle::Init(-1);
	// TTree *T = (TTree*) TFile::Open(filename) -> Get(tree_name);
	TChain *T = new TChain(tree_name);
	T -> Add(filename);
	TH1* accept = (TH1*) TFile::Open("accept_hist.root") -> Get("h");
	const char* gen_cut = "0 < bkmm_gen_tau && bkmm_gen_tau < 1e-11";
        const char* kin_cut = "0 < bkmm_jpsimc_tau && bkmm_jpsimc_tau < 1e-11";
        const char* c4 = "bkmm_jpsimc_alpha<0.125 && bkmm_kaon_pt>1.16 && bkmm_nomc_sl3d>2.34 && log(bkmm_jpsimc_vtx_chi2dof)<1.99";
	
	TH1F* h0 = new TH1F("h0", "tau", 100, 0, 10);
	TH1F* h1 = new TH1F("h1", "tau", 100, 0, 10);
	TH1F* h2 = new TH1F("h2", "tau", 100, 0, 10);
	
	// T -> Draw("1e12 * bkmm_gen_tau >> h0", gen_cut);
        // T -> Draw("1e12 * bkmm_jpsimc_tau >> h1", kin_cut);
        T -> Draw("1e12 * bkmm_jpsimc_tau >> h2", Form("%s && %s", kin_cut, c4));
		
	// h0 -> Sumw2();
	// h1 -> Sumw2();
	h2 -> Sumw2();

	h0 -> SetLineColor(1);
        h1 -> SetLineColor(2);
        h2 -> SetLineColor(3);

	// h0 -> Divide(accept);
	// h1 -> Divide(accept);
	h2 -> Divide(accept);

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

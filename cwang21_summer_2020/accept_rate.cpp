#include <iostream>
#include "TF1.h"
#include "TTree.h"
#include "TH1.h"
#include "TFile.h"
#include "TChain.h"
#include "filenames.h"

#include "MitRootStyle/MitRootStyle.C"
using namespace ROOT;
using namespace std;

void accept_rate(){
	/*TFile *f = TFile::Open(names::sim_1);
	TTree *T = (TTree* ) f -> Get("Candidates");
	TTree *runs = (TTree*) f -> Get("Runs");
	// f -> Close()
	long long int tot = 0;
	long long int evts;
	runs -> SetBranchAddress("genEventCount_", &evts);
	for (int i = 0; i < runs -> GetEntries(); i++){
		runs -> GetEntry(i);
		tot += evts;
	}*/
	TChain* C = new TChain("Candidates");
	C -> Add(names::sim_4);
	TChain* C_runs = new TChain("Runs");
	C_runs -> Add(names::sim_4);
	// C_runs -> Print();
	// C_runs -> GetTree() -> Print();
	// T -> Print();
	long long int tot = 0;
	long long int evts;
	C_runs -> SetBranchAddress("genEventCount_", &evts);
	// C_runs -> Scan();
	for (int j = 0; j < C_runs -> GetEntries(); j++){
		C_runs -> GetEntry(j);
		tot += evts;
	}
	
	cout << "Generated Total: " << tot << endl;
	// cout << "Reconstructed Total: " << T -> GetEntries() << endl;
	TH1F* h = new TH1F("h", "tau", 100, 0, 10);
	const char* gen_cut = "0 < bkmm_gen_tau && bkmm_gen_tau < 1e-11";
	C -> Draw("1e12 * bkmm_gen_tau >> h", gen_cut);
	cout << "Reconstructed Total: " << h -> GetEntries() << endl;
	TF1* mult = new TF1("mult", "[0] * exp(x / [1])", 0, 10);
	mult -> SetParameter(0, (10 * 1.638) / tot);
	mult -> SetParameter(1, 1.638);
	h -> Sumw2();
	h -> Multiply(mult, 1.);
} 

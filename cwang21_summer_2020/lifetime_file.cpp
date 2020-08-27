#include <iostream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TChain.h"
#include "filenames.h"

const char* filename = names::events_dir;
const char* tree = "Events";

void lifetime_file(){
	TChain* C = new TChain(tree);
	C -> Add(filename);
	
	const char* c4 = "bkmm_jpsimc_alpha<0.125 && bkmm_kaon_pt>1.16 && bkmm_nomc_sl3d>2.34 && log(bkmm_jpsimc_vtx_chi2dof)<1.99";
	const char* kin_cut = "0 < bkmm_jpsimc_tau && bkmm_jpsimc_tau < 1e-11";
	
	TH2F* h = new TH2F("h", "mass_v_tau", 100, 4.9, 5.9, 100, 0, 10);
	C -> Draw("1e12 * bkmm_jpsimc_tau: bkmm_jpsimc_mass >> h", Form("%s && %s", kin_cut, c4));
	TFile f("saved_hist.root", "NEW");
	h -> Write("h");
}

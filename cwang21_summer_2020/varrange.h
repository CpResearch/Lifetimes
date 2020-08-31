#pragma once 
#include <TString.h>
class vardata{
	public:
	const char* varName;
	const char* prettyName;
	const char* fileName;
	Float_t plotLower;
	Float_t plotUpper;
	Float_t cutLower;
	Float_t cutUpper;
	bool cutUp;
	vardata(const char* n1, const char* n2, const char* fn, Float_t pl, Float_t pu, Float_t cl, Float_t cu, bool d): varName(n1), prettyName(n2), fileName(fn), plotLower(pl), plotUpper(pu), cutLower(cl), cutUpper(cu), cutUp(d) {}
};
namespace vars{
	
	vardata bkmm_kaon_pt("bkmm_kaon_pt","kaon p_{T} [GeV]","bkmm_kaon_pt",1,20,1,5,true);
	
	vardata bkmm_nomc_sl3d("bkmm_nomc_sl3d","3d length significance","bkmm_nomc_sl3d", 0,20,0,6,true);
	
	vardata ln_vtx_chi2dof("log(bkmm_jpsimc_vtx_chi2dof)","ln(\\chi^{2} dof)","ln_vtx_chi2dof", -10,6,-2,5,false);
	
	// vardata bkmm_jpsimc_alpha("bkmm_jpsimc_alpha", "pointing angle", "bkmm_jpsimc_alpha", 0,3.15,0,3.15,false);
	vardata bkmm_jpsimc_alpha("bkmm_jpsimc_alpha", "pointing angle", "bkmm_jpsimc_alpha", 0, 3.15, 0, 0.5,false);
	
	vardata bkmm_bmm_bdt("bkmm_bmm_bdt", "bdt", "bkmm_bmm_bdt", -0.6,0.7,-0.5,0.5,true);
	
	
	vardata list[] = {bkmm_kaon_pt, bkmm_nomc_sl3d, ln_vtx_chi2dof, bkmm_jpsimc_alpha, bkmm_bmm_bdt};
	int numVars = 5;	
}

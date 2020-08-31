#include <iostream>
#include "TF1.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TPad.h"

using namespace ROOT;
using namespace std;

const char* filename = "exampledata.root";
const char* tree_name = "Candidates";


void TreeSet(TTree *T, const char* name, Float_t* v){
  T -> SetBranchStatus(name, 1);
  T -> SetBranchAddress(name, v);
}


Float_t Score(Float_t sl3d, Float_t mu1, Float_t mu2){
   //return 5 * log(sl3d);
   return 2.28 * log(sl3d) + 3.4 * log(mu1) + 3.4 * log(mu2);
}

void background(){
  TTree *T = (TTree *) TFile::Open(filename) -> Get(tree_name);
  T -> SetBranchStatus("*", 0);
  // bkmm_jpsimc_sl3d, Muon1_pt, Muon2_pt, bkmss_jpsimc_vtx_chi2dof
  Float_t sl3d;
  TreeSet(T, "bkmm_jpsimc_sl3d", &sl3d);
  Float_t mu1;
  TreeSet(T, "Muon1_pt", &mu1);
  Float_t mu2;
  TreeSet(T, "Muon2_pt", &mu2);
  Float_t chi2;
  TreeSet(T, "bkmm_jpsimc_vtx_chi2dof", &chi2);
  Float_t mass;
  TreeSet(T, "bkmm_jpsimc_mass", &mass);
  
  Float_t* v1 = &sl3d;
  Float_t* v2 = &mu1;
  
  TH1F h1("h1", "ploooot", 100, 4, 20);
  TH1F h2("h2", "pot", 100, 4, 20);
  TH2F h("h2", "sl3d vs Muon1_pt", 32, 4, 20, 32, 4, 20);
  TH2F hScore("hScore", "Score_hist", 100, 4.8, 5.8, 20, 10, 35);
  Long64_t nentries = T->GetEntries();
  for (Long64_t i = 0; i < nentries; i++){
    T -> GetEntry(i);
    if (!(5.24 < mass &&  mass < 5.31)) continue; //{h2.Fill(*v1); continue;}
    h.Fill(*v1, *v2);
    // h.Fill(*v2, *v1);
    h1.Fill(*v1);
    // h1.Fill(*v2);
 
    hScore.Fill(mass, Score(sl3d, mu1, mu2));
  }
  TCanvas* canvas = new TCanvas();
  // cout << h.GetCorrelationFactor() << endl;
  h.DrawCopy("LEGO");
  // h1.DrawNormalized();
  // h2.DrawNormalized("SAME");
  gPad -> Modified();
  gPad -> Update(); 
}

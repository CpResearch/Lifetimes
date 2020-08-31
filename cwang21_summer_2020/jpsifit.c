#include <iostream>
#include "TF1.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TFile.h"
#include "Math/WrappedTF1.h"
#include "Math/GaussIntegrator.h"
#include "TPad.h"
using namespace ROOT;
using namespace std;

const char* filename = "exampledata.root";
const char* branch_name = "bkmm_jpsimc_mass";
const char* tree_name = "Candidates";

Double_t backgroundFn(Double_t *x, double *par){
  return par[0] + x[0] * par[1] + x[0] * x[0] * par[2];
}

Double_t signalFn(double *x, double *par){
  return par[0] * exp(-0.5 * ((x[0] - par[1]) / par[2]) * ((x[0] - par[1]) / par[2]));
}

void TreeSet(TTree *T, const char* name, Float_t *v){
  T -> SetBranchStatus(name, 1);
  T -> SetBranchAddress(name, v);
}


Float_t Score(Float_t sl3d, Float_t mu1, Float_t mu2){
  return sl3d;
  return 2.28 * log(sl3d) + 3.4 * log(mu1) + 3.4 * log(mu2);
}


TH1F readHist(TH2F* h2){
  cout << "Reading from: " << filename  << endl;
  cout << "Using tree: " << tree_name << endl;
  cout << "Analyzing branch: " << branch_name << endl;
  // TCanvas* canvas = new TCanvas();
  TTree *T = (TTree*) TFile::Open(filename) -> Get(tree_name);
  T -> SetBranchStatus("*", 0);
  T -> SetBranchStatus(branch_name , 1);
  Float_t *mass = (Float_t *)malloc(4);
  T -> SetBranchAddress(branch_name, mass);
  TH1F h1("h1", "J/psi event mass", 100, 4.9, 5.8);
  // h1.SetDirectory(0);
  // T -> Print();
  
  const char* aux_branch_name = "bkmm_kaon_pt"; 
  const char* aux_branch_2 = "bkmm_nomc_sl3d";
  Float_t aux;
  Float_t aux2;
  T -> SetBranchStatus(aux_branch_name, 1);
  T -> SetBranchAddress(aux_branch_name, &aux);


  TreeSet(T, aux_branch_2, &aux2);
  // TreeSet(T, aux_branch_name, &aux);
  
  // TH2F h2("h2", "thingy", 100, 4.9, 5.8, 100, 0, 10);
  
  // Float_t aux2;
  // TreeSet(T, "bkmm_jpsimc_vtx_chi2dof", &aux2);
  
  // TH3F h3("h3", "thongy", 30, 4.9, 5.8, 30, 4, 20, 20, 0, 20);
  
  // TH2F h2_uncut("h2_uncut", "uncut", 50, 4, 20, 20, 0, 6); 
  // TH2F h2_cut("h2_cut", "cut", 50, 4, 20, 20, 0, 6);
  // Float_t sl3d, mu1, mu2, chi2;
  // TreeSet(T, "bkmm_jpsimc_sl3d", &sl3d);
  // TreeSet(T, "Muon1_pt", &mu1);
  // TreeSet(T, "Muon2_pt", &mu2);
  
  Long64_t nentries = T->GetEntries();
  cout << nentries << endl;
  for (Long64_t i = 0; i < nentries; i++){
    // std::cout << i << endl;
    T -> GetEntry(i);
    // cout << *mass << endl;
    h1.Fill(*mass);
    h2 -> Fill(*mass, aux * 2 + aux2);// Score(sl3d, mu1, mu2));
    // if (5.24 < mass && mass < 5.31){
    // h3.Fill(mass, aux, aux2);
    // h2_cut.Fill(aux, aux2);
    // }
    // else{
    //   h2_uncut.Fill(aux, aux2);
    // }
    // cout << aux<<(aux2) << endl;
  }
  cout << "fin" << endl;
  h1.Print();
  // h3.Print();
  // h3.DrawCopy();
  // h2 -> DrawNormalized("LEGO");
  // h2_cut.DrawNormalized("LEGO");
  gPad -> Modified();
  gPad -> Update(); 
  return  h1;
}

Float_t getQuality(TH1F* h1){
  TF1* f = new TF1("f1", "gaus(0) + pol2(3)", 4.9, 5.8);
  f -> SetParameter(0, 400); 
  f -> SetParameter(1, 5.25);
  f -> SetParameter(2, 0.02);
  f -> SetParameter(3, 100);
  f -> SetParameter(4, 0);
  f -> SetParameter(5, 0);
  f -> SetParLimits(0, 0, 1000);
  f -> SetParLimits(1, 5.2, 5.4);
  f -> SetParLimits(2, 0.01, 0.03);
  f -> SetParLimits(3, 0, 2000);
  h1 -> Fit(f, "MQ", "", 4.9, 5.8);
  // h1.Print();
  h1 -> GetYaxis()->SetRangeUser(0, 1300);
  TF1* fit = h1 -> GetFunction("f1");
  // fit -> Print();
  // std::cout << fit -> GetParameter(0) << endl;
  // h1.DrawCopy();
  TF1* signal = new TF1("signal", signalFn, 4.9, 5.8, 3);
  Double_t params[6];
  f ->  GetParameters(params);
  // std:: cout << params[2] << endl;
  signal -> SetParameters(params);
  // signal -> SetParameter(2, 0.02);
  // signal -> SetParameter(0, 400.);
  // signal -> SetParameter(1, 5.27);
  // signal -> Draw();
  // std::cout << signal -> GetParameter(0) << endl;
  // std::cout << signal -> GetParameter(1) << endl;
  // std::cout << signal -> GetParameter(2) << endl;
  // signal -> DrawCopy("LSAME");
  
  Double_t center = params[1];
  Double_t sdev = params[2];
  
  Double_t bin_width = h1 -> GetXaxis() -> GetBinWidth(69);
  // std::cout << bin_width << endl; 
  
  Math::WrappedTF1 wf(*f);
  Math::GaussIntegrator ig;
  ig.SetFunction(wf);
  ig.SetRelTolerance(0.001);
  Double_t sb = ig.Integral(center - 2 * sdev, center + 2 * sdev) / bin_width;
  Math::WrappedTF1 wsig(*signal);
  ig.SetFunction(wsig);
  Double_t s = ig.Integral(center - 2 * sdev, center + 2 * sdev) / bin_width;
  std::cout << "Base:" << sb - s << ",\t Signal + Base: " << sb << ",\tSignal: " << s 
            << ",\tQuality:" <<  s / sqrt(sb) << endl; 
  return s / sqrt(sb); 
}

void jpsifit(){
  TCanvas* canvas = new TCanvas();
  cout << "Reading from: " << filename  << endl;
  cout << "Using tree: " << tree_name << endl;
  cout << "Analyzing branch: " << branch_name << endl;
  // TCanvas* canvas = new TCanvas();
  TTree *T = (TTree*) TFile::Open(filename) -> Get(tree_name);
  
  int bin_density = 25;
  TH2F* h2 = new TH2F("h2", "thingy", 100, 4.9, 5.8, 4 * bin_density, 1, 5);
  // TH2F* h2 = new TH2F("h2", "thingy", 10, 4.9, 5.8, 10, 0, 50);
  // TH2F hScore("hScore", "hi", 100, 4.9, 5.8, 20, 13, 25);
  // readHist();
  // TH1F h1 = readHist(h2);// &hScore); 
  T -> Draw("bkmm_kaon_pt: bkmm_jpsimc_mass>>h2"); 
  //h2 -> Print();
  // h2 -> Draw("LEGO");
  //cout << "asfsd" << endl;
  //getQuality(h1); 
  // return;
  const int num_cuts = 100;
  Float_t qs[num_cuts];
  Float_t cs[num_cuts];
  Float_t min_cut = 1;
  Float_t max_cut = 5;
  for (int i = 0; i < num_cuts; i++){  
    TH1D* proj = h2 -> ProjectionX("asdf", i);//  i * bin_density * (max_cut - min_cut) / (num_cuts) + min_cut);
    // cout << proj -> GetEntries();
    TH1F* h1f = new TH1F("aaaaa", "b", 100, 4.9, 5.8);
    proj -> Copy(*h1f);
    qs[i] = getQuality(h1f);
    cs[i] = min_cut + i * (1. / bin_density);
  } 
  for (int i = 0; i < num_cuts; i++){
    cout << qs[i] << " " << i * (max_cut - min_cut) / num_cuts + min_cut << endl;
  }
  TGraph* gr = new TGraph(num_cuts, cs, qs);
  gr -> Draw("A*");
  gr -> SetTitle("Quality vs kaon_pt cut;kaon_pt cut (GeV);Quality");
  gPad -> Modified();
  gPad -> Update();
  canvas -> SaveAs("out.png");
  // gr -> Print();
  return;
}

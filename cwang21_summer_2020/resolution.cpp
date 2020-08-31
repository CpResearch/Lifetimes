#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TF1Convolution.h"

#include "filenames.h"
#include "MitRootStyle/MitRootStyle.C"


void resolution(){
	TTree* T = (TTree*) TFile::Open(names::sim_1) -> Get("Candidates");
	TH1F* h = new TH1F("h", "err", 100, -1, 1);
	T -> Draw("1e12 * (bkmm_jpsimc_tau) >> h");
	TF1* gen = new TF1("gen", "[0] * (1 + (x / [1])^2)^(-[2])", -1, 10);
	gen -> SetParameter(1, .1);
	gen -> SetParameter(2, 1.5);
	TF1* exp = new TF1("exp", "((x>0)? exp(- (x / [0])): 0)", -1, 10); 
	// exp -> SetParameter(0, 1);
	TF1Convolution* conv = new TF1Convolution(gen, exp, -1, 10, true);
	conv -> SetNofPointsFFT(1000);
	TF1* f = new TF1("f", *conv, -1, 10, conv->GetNpar());
	f -> SetParameters(1e3, .1, 1.5, 1.);
	h -> Fit("f");
	h -> Draw();
	gPad -> SetLogy();
} 

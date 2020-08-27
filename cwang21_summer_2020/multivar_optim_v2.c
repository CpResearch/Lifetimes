#include <iostream>
#include "TF1.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TFile.h"
#include "Math/WrappedTF1.h"
#include "Math/GaussIntegrator.h"
#include "TPad.h"

#include "varrange.h"
#include "MitRootStyle/MitRootStyle.C"
using namespace ROOT;
using namespace std;

const char* filename = "exampledata_3.root";
// const char* branch_name = "bkmm_jpsimc_mass";
const char* tree_name = "Events";

void makeFrame(TGraphErrors *graph, const char *xtit, const char *ytit)
{

  Double_t xMin = graph->GetX()[0];
  Double_t xMax = graph->GetX()[graph->GetN()-1];
  Double_t xDel = (xMax-xMin)/20.;
  printf(" X(min,max,delta): %f, %f, %f\n",xMin,xMax,xDel);
  Double_t yMin = 99999;
  Double_t yMax = -99999;
  for (Int_t i=0; i<graph->GetN(); i++) {
    Double_t max = graph->GetY()[i]+graph->GetEY()[i];
    Double_t min = graph->GetY()[i]-graph->GetEY()[i];
    if (max>yMax)
      yMax = max;
    if (min<yMin)
      yMin = min;
  }
  Double_t yDel = (yMax-yMin)/20.;
  printf(" Y(min,max,delta): %f, %f, %f\n",yMin,yMax,yDel);

  TH1F *frame = new TH1F("Frame","",1,xMin-xDel,xMax+xDel);
  MitRootStyle::InitHist(frame,xtit,ytit);

  frame->SetMinimum(yMin-yDel);
  frame->SetMaximum(yMax+yDel);
  frame->Draw();

  return;
}


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

struct SignalBackground{
	Float_t s, b;
};
typedef struct SignalBackground sb; 

sb getSB(TH1F* h1){
	TF1* f = new TF1("f1", "gaus(0) + pol2(3)", 4.9, 5.8);
	f -> SetParameter(0, 400); 
	f -> FixParameter(1, 5.277);
	f -> FixParameter(2, 0.018);
	f -> SetParameter(3, 100);
	f -> SetParameter(4, 0);
	f -> SetParameter(5, 0);
	f -> SetParLimits(0, 0, 1000);
	f -> SetParLimits(1, 5.2, 5.4);
	f -> SetParLimits(2, 0.01, 0.03);
	f -> SetParLimits(3, 0, 2000);
	h1 -> Fit(f, "MQ", "", 4.9, 5.8);
	h1 -> GetYaxis()->SetRangeUser(0, 1300);
	TF1* fit = h1 -> GetFunction("f1");
	TF1* signal = new TF1("signal", signalFn, 4.9, 5.8, 3);
	Double_t params[6];
	f ->  GetParameters(params);
	signal -> SetParameters(params);
	
	Double_t center = params[1];
	Double_t sdev = params[2];
	
	Double_t bin_width = h1 -> GetXaxis() -> GetBinWidth(69);
	
	Math::WrappedTF1 wf(*f);
	Math::GaussIntegrator ig;
	ig.SetFunction(wf);
	ig.SetRelTolerance(0.001);
	Double_t spb = ig.Integral(center - 2 * sdev, center + 2 * sdev) / bin_width;
	Math::WrappedTF1 wsig(*signal);
	ig.SetFunction(wsig);
	Double_t s = ig.Integral(center - 2 * sdev, center + 2 * sdev) / bin_width;
	// std::cout << "Base:" << spb - s << ",\t Signal + Base: " << spb << ",\tSignal: " << s 
	// 	        << ",\tQuality:" <<  s / sqrt(spb) << endl; 
	sb ret;
	ret.s = s;
	ret.b = spb - s;
	return ret; 
}

sb getSBSideband(TH1F* h1){
	Float_t mean = 5.277;
	Float_t sdev = 0.018;

	TAxis* axis = h1 -> GetXaxis();
	int b1 = axis -> FindBin(mean - 5 * sdev);
	int b2 = axis -> FindBin(mean - 3 * sdev);
	int b3 = axis -> FindBin(mean - 2 * sdev);
	int b4 = axis -> FindBin(mean + 2 * sdev);
	int b5 = axis -> FindBin(mean + 3 * sdev);
	int b6 = axis -> FindBin(mean + 5 * sdev);

	Double_t spb = h1 -> Integral(b3, b4);
	Double_t sidebins = h1 -> Integral(b1, b2) + h1 -> Integral(b5, b6);
	
	Double_t factor = (b4 - b3) * 1. / (b6 + b2 - b5 - b1 + 0.);  
	sb ret;
	ret.s = spb - sidebins * factor ;
	ret.b = sidebins * factor;
	cout << factor << "\t" << ret.s << "\t" << ret.b << endl;
	return ret;
}



void optimal_cut(TH2F* h2, bool cut_up, Float_t* out_best, Float_t* out_cut){
	Int_t num_bins = h2 -> GetYaxis() -> GetNbins();
	
	Float_t best = 0;
	Float_t best_cut = 0;

	for (int i=1; i<num_bins; i++){
		// cout << cut_up << endl;
		TH1D* proj = cut_up?h2 -> ProjectionX("asdf", i, -1): h2 -> ProjectionX("asdf", 0, i);
		// cout << proj -> GetEntries();
		TH1F* h1f = new TH1F("aaaaa", "b", 100, 4.9, 5.8);
		proj -> Copy(*h1f);
		sb a = getSB(h1f);
		Float_t quality = a.s / sqrt(a.s + a.b);
		Float_t cut =  h2 -> GetYaxis() -> GetBinUpEdge(i - cut_up);
		// cout << quality << "\t" << cut << "\t" << best << endl;
		if (quality > best){
			best = quality;
			best_cut = cut;
		}
	}
	*out_best = best;
	*out_cut = best_cut;	
}


void aaa(TTree* T, TH2F* h2, vardata v, TString* cut_str){
	// cout << *cut_str << endl;
	h2 -> Reset();
	T -> Draw(Form("%s: bkmm_jpsimc_mass>>%s", v.varName, h2 -> GetName()), *cut_str); 
	Float_t quality, cut;
	optimal_cut(h2, v.cutUp, &quality, &cut);
	cout << "Cutting by: " << v.prettyName << "\tQuality: " << quality << "\tCut: " << cut << endl;
	cut_str -> Form("%s %s %f", v.varName, v.cutUp?">":"<", cut);
	// cout << *cut_str << endl;
} 

void multivar_optim_v2(){
	cout << "Reading from: " << filename  << endl;
	cout << "Using tree: " << tree_name << endl;
	TTree *T = (TTree*) TFile::Open(filename) -> Get(tree_name);

	TH1F* baseline = new TH1F("baseline", "aaa", 100, 4.9, 5.8);
	T -> Draw("bkmm_jpsimc_mass>>baseline");

	sb a = getSB(baseline);
	cout << a.s / sqrt(a.s + a.b) << endl;

	const int n = 2;
	// vardata vs[n] = {vars::bkmm_jpsimc_alpha, vars::bkmm_kaon_pt, vars::bkmm_nomc_sl3d, vars::ln_vtx_chi2dof, vars::bkmm_bmm_bdt};
	vardata vs[n] = {vars::bkmm_bmm_bdt, vars::bkmm_kaon_pt,};
	// cout << v.prettyName << endl;
	// cout << "Analyzing branch: " << branch_name << endl;
	// TCanvas* canvas = new TCanvas();
	//
	TH2F* h2s[n];
	for (int i = 0; i < n; i++){
		h2s[i] = new TH2F(Form("h2_%d", i), "thingy", 100, 4.9, 5.8, 100, vs[i].cutLower, vs[i].cutUpper);
	}
	// TH2F* h2 = new TH2F("h2", "thingy", 10, 4.9, 5.8, 10, 0, 50);
	// TH2F hScore("hScore", "hi", 100, 4.9, 5.8, 20, 13, 25);
	TString cut_strs[n]; // {"2 > 1", "2 > 1"}; // , "2 > 1", "2 > 1"};
	for (int i = 0; i < n; i++){
		cut_strs[i] = "2 > 1";
	}
	for (int j = 0; j < 10; j++){
		for (int i = 0; i < n; i++){
			TString cut_str = "2 > 1";
			for (int k = 0; k < n; k++){
				if (i != k){cut_str = Form("%s&&%s", cut_str.Data(), cut_strs[k].Data());}
			}
			// cout << cut_str << endl;
			aaa(T, h2s[i], vs[i], &cut_str);
			cut_strs[i] = cut_str; 
		}
	}
	/*aaa(T, h2_1, v1, &cut_str);
	aaa(T, h2_2, v2, &cut_str);
	aaa(T, h2_1, v1, &cut_str);
	aaa(T, h2_2, v2, &cut_str);
	aaa(T, h2_1, v1, &cut_str);
	aaa(T, h2_2, v2, &cut_str);*/
	// readHist();
	// TH1F h1 = readHist(h2);// &hScore); 
	// T -> Draw(Form("%s: bkmm_jpsimc_mass>>h2", v.varName)); 
	// Float_t quality, cut;
	// optimal_cut(h2, v.cutUp, &quality, &cut);
	// cout << "Cutting by: " << v.prettyName << "\tQuality: " << quality << "\tCut: " << cut << endl;

	
}

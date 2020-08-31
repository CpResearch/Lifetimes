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
	std::cout << "Base:" << spb - s << ",\t Signal + Base: " << spb << ",\tSignal: " << s 
		        << ",\tQuality:" <<  s / sqrt(spb) << endl; 
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



void analyze_cut(TGraphErrors* graph, TH2F* h2, bool cut_up){
	Int_t num_bins = h2 -> GetYaxis() -> GetNbins();
	graph -> Set(num_bins);
	// Float_t quality[num_bins];
	// Float_t cuts[num_bins];
	// Float_t errors[num_bins];
	for (int i=0; i<num_bins; i++){
		// cout << cut_up << endl;
		TH1D* proj = cut_up?h2 -> ProjectionX("asdf", i, -1): h2 -> ProjectionX("asdf", 0, i);
		// cout << proj -> GetEntries();
		TH1F* h1f = new TH1F("aaaaa", "b", 100, 4.9, 5.8);
		proj -> Copy(*h1f);
		sb a = getSB(h1f);
		Float_t quality = a.s / sqrt(a.s + a.b);
		Float_t cut =  h2 -> GetYaxis() -> GetBinCenter(i);
		Float_t error = 2 * a.s / sqrt(a.s + a.b) * sqrt(1 / a.s + 1 / (4 * (a.s + a.b)));
		graph -> SetPoint(i, cut, quality);
		graph -> SetPointError(i, 0, error);
	}	
}

void cut_analysis(){
	MitRootStyle::Init(-1);
	cout << "Reading from: " << filename  << endl;
	cout << "Using tree: " << tree_name << endl;
	TTree *T = (TTree*) TFile::Open(filename) -> Get(tree_name);
	for (int i = 0; i < vars::numVars; i++){
	TCanvas* c = new TCanvas();
	vardata v = vars::list[i];
	cout << v.prettyName << endl;
	// cout << "Analyzing branch: " << branch_name << endl;
	// TCanvas* canvas = new TCanvas();

	TH2F* h2 = new TH2F("h2", "thingy", 100, 4.9, 5.8, 100, v.cutLower, v.cutUpper);
	// TH2F* h2 = new TH2F("h2", "thingy", 10, 4.9, 5.8, 10, 0, 50);
	// TH2F hScore("hScore", "hi", 100, 4.9, 5.8, 20, 13, 25);
	// readHist();
	// TH1F h1 = readHist(h2);// &hScore); 
	T -> Draw(Form("%s: bkmm_jpsimc_mass>>h2", v.varName)); 
	//h2 -> Print();
	// h2 -> Draw("LEGO");
	//cout << "asfsd" << endl;
	//getQuality(h1); 
	// return;
	TGraphErrors* graph = new TGraphErrors();
	MitRootStyle::InitGraph(graph, "", "", kBlue);
	analyze_cut(graph, h2, v.cutUp);
	makeFrame(graph, v.prettyName, "quality");
	graph -> Draw("PESame");
	c -> SaveAs(Form("cut_analysis_%s.png", v.fileName));
	}
	/*
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
		sb a = getSB(h1f);
		qs[i] = a.s / sqrt(a.s + a.b);
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
	// canvas -> SaveAs("out.png");
	// gr -> Print();
	return;*/ 
}

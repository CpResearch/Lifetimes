#include <iostream>
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "filenames.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "MitRootStyle/MitRootStyle.C"

struct TauError{Float_t tau; Float_t error;};
typedef struct TauError te;

te GetTau(TH1D* h, Float_t start){
	TF1* f = new TF1("f1", "[0] * exp(-x / [1])", start, 10);
	f -> SetParameter(1, 2.);
	h -> Fit(f, "", "", start, 10);
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

void tau_analysis(){
	TH2F* h = (TH2F*) TFile::Open("saved_hist.root") -> Get("h");
	// h -> Draw();
	MitRootStyle::Init(-1);
	cout << "1" << endl;	
	TH1D* in_band = h -> ProjectionY("a", 524-490, 530-490);
	// MitRootStyle::InitHist(in_band, "Lifetime [s]", "Counts");
	cout << "2" << endl;
	TH1D* out_a = h -> ProjectionY("b", 505-490, 515-490);
	cout << "3" << endl;
	TH1D* out_b = h -> ProjectionY("c", 540 - 490, 550 - 490);
	cout << "4" << endl;
	// out_a -> Add(out_b);
	cout << "4.5";
	TCanvas* c = new TCanvas();
	// 
	MitRootStyle::InitHist(out_a, "Lifetime [ps]", "Counts");
	 gPad -> SetLogy();
	out_a -> Draw();
	out_a -> SetLineColor(1);
	out_b -> SetLineColor(2);
	out_b -> Draw("SAME");
	// gPad -> SetRightMargin(.15);
	// TLegend* legend = new TLegend(0.5, .75, .85, .96);
	TLegend* legend = new TLegend(0.51, .7, .9, .9);
	legend -> AddEntry(out_a, "Left Sideband");
	legend -> AddEntry(out_b, "Right Sideband");
	legend -> Draw();
	// in_band -> Draw("SAME");
	// in_band -> SetLineColor(2);
	te result = GetTau(in_band, 1.);
	cout << result.tau << "\t" << result.error << endl;
	cout << "5" << endl;
}

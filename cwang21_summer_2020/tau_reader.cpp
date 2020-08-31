#include <iostream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "filenames.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "MitRootStyle/MitRootStyle.C"



void tau_reader(){
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
	MitRootStyle::InitHist(out_a, "Lifetime [s]", "Counts");
	 gPad -> SetLogy();
	out_a -> Draw();
	out_a -> SetLineColor(1);
	out_b -> SetLineColor(2);
	out_b -> Draw("SAME");
	gPad -> SetRightMargin(.15);
	TLegend* legend = new TLegend(0.5, .75, .85, .96);
	legend -> AddEntry(out_a, "Left Sideband");
	legend -> AddEntry(out_b, "Right Sideband");
	legend -> Draw();
	// in_band -> Draw("SAME");
	// in_band -> SetLineColor(2);
	cout << "5" << endl;
}

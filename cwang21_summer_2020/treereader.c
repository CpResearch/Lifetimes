#include <string.h>
using namespace ROOT;
void treereader(){
  TFile* f = TFile::Open("example_data_2.root");
  TTreeReader r("Events", f);
  TObjArray* branches = r.GetTree() -> GetListOfBranches();
  //branches -> Print();
  TObjArrayIter iter(branches);
  TBranch* b;
  while ((b = (TBranch*)iter())){
    //  if ( strcmp(b -> GetTitle(), "Trigger/flag bit") == 0) continue;
    std::cout << b -> GetName() << endl;
  }
  b = r.GetTree() -> GetBranch("bkmm_jpsimc_mass");
  b -> Print();
}

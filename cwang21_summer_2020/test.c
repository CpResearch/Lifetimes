#include "TCanvas.h"
#include "TROOT.h"
#include "TF1.h"

// using namespace ROOT;
void test(){
  TCanvas* canvas_2 = new TCanvas();
  TF1 f1("f1", "x * x", 0., 10.);
  f1.DrawClone("L");
  // canvas_2.Draw();
  // std::cout << "hi"  << endl;
}

int main(){
  test();
}

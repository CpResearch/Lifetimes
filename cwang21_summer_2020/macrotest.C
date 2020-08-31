#include "TCanvas.h"
#include "TROOT.h"
#include "TF1.h"

void macrotest(){
    auto  mycanvas = new TCanvas();
    TF1 f("Linear law","1+x*1",.5,10.5);
    f.DrawClone("APE");
}

int main(){
macrotest();
}

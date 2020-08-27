#include "TCanvas.h"
#include "TROOT.h"
#include "TF1.h"

void test2(){
    auto  mycanvas = new TCanvas();
    TF1 f("Linear law","1+x*1",.5,10.5);
    f.DrawClone("L");
}

int main(){
    test2();
}

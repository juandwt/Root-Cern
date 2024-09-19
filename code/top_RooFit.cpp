#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooChebychev.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "RooPlot.h"

using namespace RooFit;

void ajuste2(){
    
    RooRealVar x("x", "x", 0, 10);

    RooRealVar mean("mean", "mean of gaussians", 5);
    RooRealVar sigma1("sigma1", "width of gaussians", 0.5);

    RooGaussian sig1("sig1", "Signal component 1", x, mean, sigma1);

    RooRealVar a0("a0", "a0", -1);
    RooRealVar a1("a1", "a1", -0.2, -1, 0.1);
    RooChebychev bkg("bkg", "Background", x, RooArgSet(a0, a1));
   
    RooRealVar sig1frac("sig1frac", "fraction of component 1 in signal", 0.8, 0., 1.);
    RooAddPdf sig("sig", "Signal", RooArgList(sig1), sig1frac);

    RooRealVar nsig("nsig", "number of signal events", 500, 0, 10000);
    RooRealVar nbkg("nbkg", "number of background events", 500, 0, 10000);
    RooAddPdf model("model", "(g1)+a", RooArgList(bkg, sig), RooArgList(nbkg, nsig));

    RooDataSet *data = model.generate(x);
    model.fitTo(*data);

    RooPlot* xframe = x.frame(Title("Gauss + Pol"));
    data->plotOn(xframe);
    
    model.plotOn(xframe, Normalization(1.0, RooAbsReal::RelativeExpected), LineColor(kRed));
    model.plotOn(xframe, Components(bkg), LineStyle(kDashed), Normalization(1.0, RooAbsReal::RelativeExpected), LineColor(kGreen));
    model.plotOn(xframe, Components(RooArgSet(sig1)), LineStyle(kDotted), Normalization(1.0, RooAbsReal::RelativeExpected));

    // Calcular el residuo 
    RooHist *hresid = xframe->residHist();

    // Calcular el pull
    RooHist *hpull  = xframe->pullHist();

    //Graficar el residuo
    RooPlot* frame2 = x.frame(Title("Residual Distribution"));    
    frame2->addPlotable(hresid, "P");

    sig1.plotOn(frame2, LineColor(kBlue), LineStyle(kDotted));

    // graficar el pull
    RooPlot* frame3 = x.frame(Title("Pull Distribution"));
    frame3->addPlotable(hpull, "P");
    
    TCanvas *c = new TCanvas("residual", "residual", 1200, 450);
    c->Divide(3);
    
    c->cd(1);
    xframe->Draw();
    c->cd(2);
    frame2->Draw();
    c->cd(3);
    frame3->Draw();
}

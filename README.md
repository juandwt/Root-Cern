# Root-Cern

<p align="center">
  <img width="800" height="250" src="rootlogo.png">
</p>

## Add histograms 

```cpp
#include <TCanvas.h>
#include <TH1F.h>
#include <TRandom3.h>
#include <TF1.h> 
#include <TLegend.h>

void Double_slick() {
  
  TLegend *leg = new TLegend(0.7, 0.7, 0.9, 0.9);  
  TCanvas *canvas  = new TCanvas("canvas", "Histogram Canvas", 900, 500); 

  TH1F *histogram1  = new TH1F("histogram", "Histogram's Fit", 100, -30, 120);
  TH1F *histogram2 = new TH1F("histogram2", "Random Numbers Histogram 2", 100, -30, 120);
  TH1F *histogramSum = new TH1F("histogramsum", "Random Numbers Histogram Sum", 100, -30, 120);
  
  histogram1->SetStats(kFALSE);
  histogram2->SetStats(kFALSE);
  histogramSum->SetStats(kFALSE);

  TRandom3 randomGenerator; 
  TRandom3 randomGenerator2;

  double randomNumber = 0;
  double randomNumber2 = 0;

  for (int i = 0; i < 3000; ++i) {
    randomNumber = randomGenerator.Gaus(20, 10); 
    histogram1->Fill(randomNumber);
  } 

  for (int i = 0; i < 3000; ++i){
    randomNumber2 = randomGenerator.Gaus(60, 10);
    histogram2->Fill(randomNumber2);
  }

  histogramSum->Add(histogram1);
  histogramSum->Add(histogram2);

  TF1 *gaussFit = new TF1("gaussFit", "gaus", -50, 120);
  histogram1->Fit(gaussFit, "R");

  TF1 *gaussFit2 = new TF1("gaussFit2", "gaus", -50, 120);
  histogram2->Fit(gaussFit2, "R");

  TF1 *gaussFit3 = new TF1("gaussFit3", "gaus", -50, 120);
  histogramSum->Fit(gaussFit3, "R");

  gaussFit->SetLineColor(kBlue);
  gaussFit2->SetLineColor(kGreen);
  gaussFit3->SetLineColor(kRed);

  histogram1->Draw();
  histogram2->Draw("SAME");
  histogramSum->Draw("SAME");

  gaussFit->Draw("SAME");
  gaussFit2->Draw("SAME");
  gaussFit3->Draw("SAME");

 leg->AddEntry(histogram1, "Histogram 1", "l");
 leg->AddEntry(gaussFit, "Ajuste 1", "l");
 leg->AddEntry(histogram2, "Histogram 2", "l");
 leg->AddEntry(gaussFit2, "Ajuste 2", "l");
 leg->AddEntry(histogramSum, "Sum Histogram", "l");
 leg->AddEntry(gaussFit3, "Ajuste Sum", "l");

 // Dibujar la leyenda una sola vez
 leg->Draw();
}

```

<p align="center">
  <img width="700" height="400" src="/Images/histogramsadd.jpg">
</p>



## Roofit  Gaus + Pol

```cpp
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
 
    // Dominio 
    RooRealVar x("x", "x", 0, 10);
    
    // Parametros de la función gauss (media, desviacion)
    RooRealVar mean("mean", "mean of gaussians", 5);
    RooRealVar sigma1("sigma1", "width of gaussians", 0.5);

    // Creación de la señal gausiana
    RooGaussian sig1("sig1", "Signal component 1", x, mean, sigma1);

    // Parametros del polinomio
    RooRealVar a0("a0", "a0", -1);
    RooRealVar a1("a1", "a1", -0.2, -1, 0.1);

    // Creación del polinomio Chebychev p(x) = a0 + a1*x
    RooChebychev bkg("bkg", "Background", x, RooArgSet(a0, a1));
   
    // Definición de una variable real para la fracción del componente 1 en la señal
    RooRealVar sig1frac("sig1frac", "fraction of component 1 in signal", 0.8, 0., 1.);
    RooAddPdf sig("sig", "Signal", RooArgList(sig1), sig1frac);
    
    // Definición de una variable real para el número de eventos de señal
    RooRealVar nsig("nsig", "number of signal events", 500, 0, 10000);
    RooRealVar nbkg("nbkg", "number of background events", 500, 0, 10000);
    
    // Creación del modelo total que combina el fondo y la señal con sus respectivas cantidades
    RooAddPdf model("model", "(g1)+a", RooArgList(bkg, sig), RooArgList(nbkg, nsig));
    
    // Generación de un conjunto de datos basado en el modelo
    RooDataSet *data = model.generate(x);
    
    // Ajuste del modelo a los datos generados
    model.fitTo(*data);
    
    // Creación de un marco para graficar
    RooPlot* xframe = x.frame(Title("Gauss + Pol"));
    
    data->plotOn(xframe);
 
    // Grafiar polinomio, gausiana y el modelo ajusatdo
    model.plotOn(xframe, Normalization(1.0, RooAbsReal::RelativeExpected), LineColor(kRed));
    model.plotOn(xframe, Components(bkg), LineStyle(kDashed), Normalization(1.0, RooAbsReal::RelativeExpected), LineColor(kGreen));
    model.plotOn(xframe, Components(RooArgSet(sig1)), LineStyle(kDotted), Normalization(1.0, RooAbsReal::RelativeExpected));
    
    TCanvas *c = new TCanvas("residual", "residual", 800, 450);
    xframe->Draw();
    c->SaveAs("Gaus + Pol.png");
}
```
The result of the code is:

<p align="center">
  <img width="700" height="280" src="/Images/gauus+pol.jpg">
</p>

##  hresid and hpull

- **Residual**: It is the difference between the observed values and the values predicted by the model. It indicates how well the model fits the data. In the code, it is calculated using `RooHist *hresid = xframe->residHist();`.

- **Pull**: It is the normalized residual, calculated by dividing the residual by the uncertainty of the fit. Pull values are expected to be within \(-3\) to \(3\) sigmas in an ideal fit, suggesting a good adjustment. If they are outside this range, it may indicate problems with the model or the presence of outliers. In the code, it is calculated using `RooHist *hpull = xframe->pullHist();`.

```cpp
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
```
The result of the code is:

<p align="center">
  <img width="1000" height="270" src="/Images/pullandresid.jpg">
</p>

# Root-Cern

<p align="center">
  <img width="800" height="290" src="rootlogo.png">
</p>

## My fiist graph in Root 

```cpp
#include <TCanvas.h>     // Incluye la clase TCanvas para crear lienzos gráficos.
#include <TF1.h>         // Incluye la clase TF1 para definir y dibujar funciones matemáticas.
#include <TApplication.h> // Incluye la clase TApplication para gestionar la aplicación ROOT.
#include <TGraph2D.h>
#include <TH3F.h>
#include <TMath.h>
#include <TRandom.h>
#include <TColor.h>


void Canvas(){
    TCanvas *C = new TCanvas("C", "Canvas", 800, 500);
    TF1 *sineFunc = new TF1("sineFunc", "sin(x)", -2*TMath::Pi(), 2*TMath::Pi());
    sineFunc->Draw();
    sineFunc->SetLineColor(kBlue);
    sineFunc->SetLineWidth(2);
    //C->Draw();
}
```
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
  <img width="700" height="300" src="/Images/gauus+pol.jpg">
</p>


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

    RooHist *hresid = xframe->residHist();
    RooHist *hpull  = xframe->pullHist();

    // Graficar el residual
    RooPlot* frame2 = x.frame(Title("Residual Distribution"));
    
    frame2->addPlotable(hresid, "P");

    // Agregar sig1 al gráfico de residuales
    sig1.plotOn(frame2, LineColor(kBlue), LineStyle(kDotted));

    RooPlot* frame3 = x.frame(Title("Pull Distribution"));
    frame3->addPlotable(hpull, "P");
    
    // Crear el canvas
    TCanvas *c = new TCanvas("residual", "residual", 1200, 450);
    c->Divide(3);
    
    // Graficar el ajuste completo
    c->cd(1);
    xframe->Draw();

    // Graficar los residuales con sig1
    c->cd(2);
    frame2->Draw();

    // Graficar la distribución "pull"
    c->cd(3);
    frame3->Draw();
}
```


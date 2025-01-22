# Table of Contents

- [My first graph in ROOT](#My-fisrt-graph-in-ROOT)
  - [Graph in 2D](#Graph-in-2D)
  - [Graph in 3D](#Graph-in-3D)
  - [Histogram in 2D and 3D](#Histogram-in-2D-and-3D)
    



- [Root-Cern](#root-cern)
  - [Add histograms](#add-histograms)
  - [Roofit Gaus + Pol](#roofit-gaus--pol)
  - [hresid and hpull](#hresid-and-hpull)
  - [Corrección](#corrección)
  - [Ajuste de masa de Lambda(2880)](#Ajuste de masa de Lambda(2880))
  - [Daliz Plot](#Daliz Plot)


# My first graph in ROOT



# Root-Cern

<p align="center">
  <img width="800" height="250" src="/Images/rootlogo.jpg">
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

## Corrección 

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
    
    // ==================================
    // Definition of functions and signal
    // ==================================

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
   
   // Model 

    RooAddPdf model("model", "(g1)+ a", RooArgList(bkg, sig), RooArgList(nbkg, nsig));
    
    RooDataSet *data = model.generate(x);
    
    model.fitTo(*data);


    RooPlot* xframe = x.frame(Title("Gauss + Pol"));
    RooPlot* frame2 = x.frame(Title("Residual Distribution"));    
    RooPlot* frame3 = x.frame(Title("Pull Distribution"));

    // ======== 
    // Graph 1
    // ========

    data->plotOn(xframe);
    model.plotOn(xframe, Normalization(1.0, RooAbsReal::RelativeExpected), LineColor(kBlue));

    
    // ======== 
    // Graph 2
    // ========

    RooHist *hresid = xframe->residHist();

    frame2->addPlotable(hresid, "P");
    model.plotOn(frame2, Normalization(1.0, RooAbsReal::RelativeExpected), Components("sig"), LineStyle(kDotted), LineColor(kRed));

    // ======== 
    // Graph 3
    // ========

    RooHist *hpull  = xframe->pullHist();
    frame3->addPlotable(hpull, "P");

    // ====== 
    // Canvas 
    // ======

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

## Decaimientos y coliciones 


```cpp
#include <TRandom.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TGenPhaseSpace.h>


void PhaseSpace_2() {
    TRandom random(10000);
    TLorentzVector W(0.0, 0.0, 0.0, 939.565); // px py pz E/C
    Double_t masses[2] = {908.27, 0.51};

    TGenPhaseSpace event;
    event.SetDecay(W, 2, masses);

    TH1F *h1 = new TH1F("h1", "h1", 100, 935, 945);

    for (Int_t n = 0; n < 100000; n++) {
        Double_t weight = event.Generate();
        
        TLorentzVector *pProton = event.GetDecay(0);   // (px, py, pz, E/C)
        TLorentzVector *pElectron = event.GetDecay(1); // (px, py, pz, E/C)
        
        TLorentzVector pNeutron = *pProton + *pElectron;
        double r = random.BreitWigner(0, 0.0005);

        // Masa invariante
        h1->Fill((1 + r) * pNeutron.M());
    }
    h1->Draw();
}
```

## Ajuste de masa de Lambda(2880)

```cpp
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooChebychev.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooDataHist.h"
#include "TCanvas.h"
#include "RooPlot.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TAxis.h"

using namespace RooFit ;
using namespace RooStats ;

void plot_tree(){
    TFile * f = new TFile("r2.root","READ");
    TTree* tree1 = (TTree*)f->Get("tree");

    Double_t  D_PX;
    Double_t  D_PY;
    Double_t  D_PZ;
    Double_t  P_PX;
    Double_t  P_PY;
    Double_t  P_PZ;
    Double_t  E_P;
    Double_t  E_D;

    // Mass
    
    Double_t M_P;
    Double_t M_D;

    tree1->SetBranchAddress("P_PX", &P_PX);
    tree1->SetBranchAddress("P_PY", &P_PY);
    tree1->SetBranchAddress("P_PZ", &P_PZ);
    tree1->SetBranchAddress("D_PX", &D_PX);
    tree1->SetBranchAddress("D_PY", &D_PY);
    tree1->SetBranchAddress("D_PZ", &D_PZ);
    

    int nbins=45;

    TH1F * DOp = new TH1F("DOp","Espectro de Masa", nbins, 2830, 2910);
    DOp->Sumw2();

    Int_t nentries = (Int_t)tree1->GetEntries();

    // Gausiana
    
    RooRealVar x("x", "x", 2830, 2910);
    RooRealVar mean("mean", "mean of gaussians", 5);
    RooRealVar sigma("sigma", "width of gaussians", 0.1);
    RooGaussian gauss("gauss", "gauss(x, mena, sigma)", x, mean, sigma);
    
    RooPlot* frame  = x.frame(Title("Gauss + Pol"));
    
    for (Int_t i=0; i< nentries;i++) {

          Double_t  E_P;
          Double_t  E_D;

          E_P = sqrt(P_PX*P_PX + P_PY*P_PY + P_PZ*P_PZ  + pow(938, 2));
          E_D = sqrt(D_PX*D_PX + D_PY*D_PY + D_PZ*D_PZ  + pow(1860, 2));

          TLorentzVector  D(D_PX, D_PY, D_PZ, E_D);
          TLorentzVector  p(P_PX, P_PY, P_PZ, E_P);
          TLorentzVector  P_T = D + p;         
         
          tree1->GetEntry(i);
          DOp->Fill(P_T.M());       
    }

    DOp->Draw();
}

void ajuste_espectro() {
    // Carga el archivo ROOT y el árbol
    TFile* f = new TFile("r2.root", "READ");
    TTree* tree1 = (TTree*)f->Get("tree");

    Double_t D_PX, D_PY, D_PZ, P_PX, P_PY, P_PZ;
    tree1->SetBranchAddress("P_PX", &P_PX);
    tree1->SetBranchAddress("P_PY", &P_PY);
    tree1->SetBranchAddress("P_PZ", &P_PZ);
    tree1->SetBranchAddress("D_PX", &D_PX);
    tree1->SetBranchAddress("D_PY", &D_PY);
    tree1->SetBranchAddress("D_PZ", &D_PZ);

    // Crear el histograma
    int nbins = 45;
    TH1F* DOp = new TH1F("DOp", "Espectro de Masa", nbins, 2830, 2910);
    DOp->Sumw2();

    Int_t nentries = tree1->GetEntries();
    for (Int_t i = 0; i < nentries; i++) {
        tree1->GetEntry(i);

        Double_t E_P = sqrt(P_PX * P_PX + P_PY * P_PY + P_PZ * P_PZ + pow(938, 2));
        Double_t E_D = sqrt(D_PX * D_PX + D_PY * D_PY + D_PZ * D_PZ + pow(1860, 2));

        TLorentzVector D(D_PX, D_PY, D_PZ, E_D);
        TLorentzVector p(P_PX, P_PY, P_PZ, E_P);
        TLorentzVector P_T = D + p;

        DOp->Fill(P_T.M());
    }

    RooRealVar x("x", "Masa Invariante", 2840, 2910);
    RooDataHist data("data", "Dataset from DOp", x, Import(*DOp));
    
    RooRealVar mean("mean", "mean of Gaussian", 2840, 2840, 2910);
    RooRealVar sigma("sigma", "width of Gaussian", 10, 0.1, 50);
    RooGaussian signal("signal", "Gaussian Signal", x, mean, sigma);

    RooRealVar mean2("mean2", "mean of Gaussian", 2840, 2850, 2860);
    RooRealVar sigma2("sigma2", "width of Gaussian", 10, 0.0001, 50);
    RooGaussian signal2("signal2", "Gaussian Signal2", x, mean2, sigma2);


    RooRealVar a0("a0", "a0", 0, -1, 1);
    RooRealVar a1("a1", "a1", 0, -1, 1);
    RooRealVar a2("a2", "a2", 0, -1, 1);
    RooRealVar a3("a3", "a3", 0, -1, 1);
    //RooChebychev background("background", "Polynomial Background", x, RooArgList(a0, a1, a2, a3));

    RooRealVar tau("tau", "tau", 0, -1, 1);  // El parámetro tau debe ser negativo para un decaimiento
    RooExponential background("background", "Exponential Background", x, tau);


    RooRealVar nsig("nsig", "number of signal events", 500, 0, 100000);
    RooRealVar nbkg("nbkg", "number of background events", 500, 0, 100000);
    RooAddPdf model("model", "signal + background", RooArgList(signal, background), RooArgList(nsig, nbkg));

    model.fitTo(data);

    RooPlot* frame  = x.frame(Title("Componentes"));
    RooPlot* frame2 = x.frame(Title("Ajuste del Espectro"));
    RooPlot* frame3 = x.frame(Title("Pull Distribution"));

    data.plotOn(frame2);

    model.plotOn(frame2, LineColor(kRed));
    model.plotOn(frame, Components(signal), LineStyle(kDashed), LineColor(kBlue));
    model.plotOn(frame, Components(background), LineStyle(kDotted), LineColor(kGreen));


    RooHist *hpull  = frame2->pullHist();
    frame3->addPlotable(hpull, "P");

    
    TCanvas* c = new TCanvas("c", "Ajuste del Espectro", 1200, 450);
    c->Divide(3);

    c->cd(1);
    frame->Draw();

    c->cd(2);
    frame2->Draw();

    c->cd(3);
    frame3->Draw();

    c->SaveAs("Ajuste_Espectro.png");
}
```

<p align="center">
  <img width="980" height="300" src="/Images/Ajuste_Espectro.jpg">
</p>



## Daliz Plot

```cpp
#include <TRandom.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <TGenPhaseSpace.h>


void PhaseSpace_2() {
    TRandom random(10000);

    TLorentzVector W(0.0, 0.0, 0.0, 1.868); // px py pz E/C
    Double_t masses[3] = {0.139, 0.139, 0.139};

    TGenPhaseSpace event;
    event.SetDecay(W, 3, masses);

    TH2F *h1 = new TH2F("h1", "h1", 50, 0, 3.5, 50, 0, 3.5);
    TH1F *h2 = new TH1F("h2", "h2", 50, 0, 3.5);

    for (Int_t n = 0; n < 100000; n++) {
        Double_t weight = event.Generate();
        
        TLorentzVector *pPion_1 = event.GetDecay(0);   // (px, py, pz, E/C)
        TLorentzVector *pPion_2 = event.GetDecay(1);   // (px, py, pz, E/C)
        TLorentzVector *pPion_3 = event.GetDecay(2);   // (px, py, pz, E/C)
        
        TLorentzVector m_12 = *pPion_1 + *pPion_2;
        TLorentzVector m_13 = *pPion_1 + *pPion_3;
     

        cout<<m_12.M()<<" "<<m_13.M()<<endl;
        h1->Fill(m_12.M2(), m_13.M2(), weight);
        h2->Fill(m_12.M2(), weight);
    }

    h2->Draw();
}


```


<p align="center">
  <img width="500" height="320" src="/Images/daliz.jpg">
</p>


# Root-Cern

<p align="center">
  <img with="150" height="290" src="rootlogo.png">
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

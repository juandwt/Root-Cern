#include <TCanvas.h>
#include <TF1.h>
#include <TGraph.h>
#include <TRandom.h>
#include <cmath>

void ajuste() {

    TCanvas *c1 = new TCanvas("c1", "Ajuste de datos", 800, 450);

    double sigma = 1.0;     
    double amplitud = 15.0; 
    double pendiente = -0.2;
    double intercepto = 1.0;

    double xmin = -10.0;  
    double xmax = 10.0;   
    double paso = 0.09;   

    int n = static_cast<int>((xmax - xmin) / paso) + 1; // numero de puntos
    // static_cast<int> trasforma en un entero el resultado

    double x[n];
    double y[n];
    double z[n];

    for (int i = 0; i < n; ++i) {
        x[i] = xmin + i * paso; 

        double gaussiana = amplitud * exp(-x[i] * x[i] / (2 * sigma * sigma));
        double polinomio = pendiente * x[i] + intercepto;

        y[i] = gaussiana + polinomio;
        z[i] = y[i] + gRandom->Uniform(-2, 2); 

    }

    TGraph *gr = new TGraph(n, x, z);

    gr->SetTitle("Gauss + Pol ;x;y");
    gr->SetMarkerStyle(20);  
    gr->SetMarkerColor(kBlack); 
    gr->Draw("AP"); // Dibuja los sin unirlos
    gr->SetMarkerSize(0.7);

    // Ajuste de los datos

    TF1 *fitFunc = new TF1("fitFunc", "[0] * exp(-x*x / (2*[1]*[1])) + [2]*x + [3]", xmin, xmax);   
    fitFunc->SetParameters(15, 1, -0.5, 1);
    
    gr->Fit(fitFunc, "R");

    // Quitar valor a los puntos atípicos

    TF1 *fitLine = (TF1*) gr->GetFunction("fitFunc");  

    fitLine->SetLineColor(kBlue);   
    fitLine->Draw("SAME"); 
}

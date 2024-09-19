#include <RooRealVar.h>      // Incluye la clase para manejar variables reales en RooFit
#include <RooDataSet.h>      // Incluye la clase para manejar conjuntos de datos
#include <RooGaussian.h>     // Incluye la clase para definir funciones de distribución Gaussiana
#include <TCanvas.h>         // Incluye la clase para crear un lienzo donde dibujar los gráficos
#include <RooPlot.h>         // Incluye la clase para crear gráficos en RooFit
#include <TAxis.h>           // Incluye la clase para manipular los ejes en ROOT
#include <TH1F.h>
#include <TCanvas.h>
#include <TRandom.h>
#include <TGraph.h>


using namespace RooFit;       // Usa el espacio de nombres de RooFit para evitar escribir RooFit:: en cada llamada

void Graph() {
    // Define una variable continua 'x' con el rango [-10, 10]
    RooRealVar x("x", "x", -5, 5);
    
    // Define la media 'mean' de la distribución Gaussiana en el rango [-10, 10]
    RooRealVar mean("mean", "mean of gaussian", 0, -10, 5);
    
    // Define la desviación estándar 'sigma' de la Gaussiana, con valores entre 0.1 y 10
    RooRealVar sigma("sigma", "width of gaussian", 1, 0.1, 10);
    
    // Define el número de eventos de la señal 'nsig' en el rango [0, 1000]
    RooRealVar nsig("nsig", "number of signal events", 500, 0, 1000);
    
    // Define una función de densidad de probabilidad Gaussiana con las variables 'x', 'mean', y 'sigma'
    RooGaussian gauss("gauss", "gaussian pdf", x, mean, sigma);
    
    // Crea una versión extendida de la Gaussiana para ajustar al número total de eventos 'nsig'
    RooExtendPdf esig("esig", "extended signal pdf", gauss, nsig);

    // Crea un histograma con 50 bins en el rango [-10, 10] para almacenar números aleatorios
    auto h1f = new TH1F("h1f", "Random Number", 50, -10, 10);
    float xmain;

    // Establece una semilla aleatoria para generar números reproducibles
    gRandom->SetSeed();

    // Bucle para generar 1000 números aleatorios con distribución Gaussiana de media 0 y sigma 1.0
    for (int i = 0; i < 1000; i++) {
        xmain = gRandom->Gaus(0, 1.0);  // Genera un número aleatorio Gaussiano
        h1f->Fill(xmain);               // Rellena el histograma con los valores generados
    }

    // Convierte el histograma en un conjunto de datos binned compatible con RooFit
    RooDataHist dh("dh", "binned version of data", RooArgSet(x), h1f);

    // Genera un conjunto de datos a partir de la Gaussiana con 1000 eventos aleatorios
    std::unique_ptr<RooDataSet> data{gauss.generate(x, 1000)};

    // Crea un gráfico en el eje 'x' con el título "gaussian pdf with data"
    RooPlot *xframe2 = x.frame(Title("gaussian pdf with data"));

    // Dibuja el histograma de datos sobre el gráfico
    dh.plotOn(xframe2);

    // Dibuja la función ajustada Gaussiana sobre los datos
    esig.plotOn(xframe2);

    // Ajusta la Gaussiana extendida a los datos del histograma
    esig.fitTo(dh);

    // Imprime los parámetros ajustados: media, sigma y número de eventos
    mean.Print();
    sigma.Print();
    nsig.Print();

    // Crea un lienzo para dibujar el gráfico y ajusta los márgenes
    TCanvas *c = new TCanvas("rf101_basics", "rf101_basics", 800, 400);
    c->cd();                                 // Activa el lienzo para dibujar
    gPad->SetLeftMargin(0.15);               // Ajusta el margen izquierdo del gráfico
    xframe2->GetYaxis()->SetTitleOffset(1.6); // Ajusta la posición del título del eje Y

    // Dibuja el gráfico en el lienzo
    xframe2->Draw();
}

// RootFit multiples adjustment


void gausiana() {
    RooRealVar x("x", "x", -10, 10);
    
    RooRealVar mean("mean", "mean of gaussian", 0, -10, 10);
    RooRealVar sigma("sigma", "width of gaussian", 1, 0.1, 10);
    RooGaussian gauss("gauss", "gaussian pdf", x, mean, sigma);

    RooDataSet data("data", "dataset with gaussian", RooArgSet(x));
    gRandom->SetSeed(0);  // Establecer una semilla para la reproducibilidad
    
    for (int i = 0; i < 1000; ++i) {
        double x_value = gRandom->Gaus(0, 1);  // Genera valores gaussianos con media 5 y sigma 1
        x.setVal(x_value);  // Asigna el valor generado a 'x'
        data.add(RooArgSet(x));  // Añade el punto al conjunto de datos
    }

    RooPlot *xframe = x.frame(Title("Gaussian Fit"));
    data.plotOn(xframe);
    
    //gauss.fitTo(data);
    //gauss.plotOn(xframe);

    // Crea un lienzo y dibuja el gráfico
    TCanvas *c = new TCanvas("c", "c", 800, 500);
    xframe->Draw();
}

void recta() {
    TCanvas *c = new TCanvas("c", "c", 800, 600);

    double slope = -0.5;
    double intercept = -1;

    const int nPoints = 100;
    double x[nPoints];
    double y[nPoints];

    gRandom->SetSeed(0);  // Establecer una semilla para la reproducibilidad

    for (int i = 0; i < nPoints; ++i) {
        x[i] = gRandom->Uniform(-10, 10);  // Generar un valor aleatorio para 'x'
        y[i] = slope * x[i] + intercept;  // Calcular 'y' en la recta
    }

    TGraph *graph = new TGraph(nPoints, x, y);
    graph->SetTitle("Datos en Forma de Recta;X;Y");
    graph->SetMarkerStyle(6);  // Estilo del marcador para los puntos
    graph->Draw("AP");
    //c->SaveAs("recta.png");
}

void ajuste(){

    TCanvas *c = new TCanvas("c", "c", 800, 500);
    
    // Gausiana

    RooRealVar x("x", "x", 2, 4);
    RooRealVar mean("mean", "mean of gaussian", 5, 1, 10);
    RooRealVar sigma("sigma", "width of gaussian", 1, 0.1, 10);
    RooGaussian gauss("gauss", "gaussian pdf", x, mean, sigma);

    RooDataSet data("data", "dataset with gaussian", RooArgSet(x));
    
    gRandom->SetSeed(0);  // Establecer una semilla para la reproducibilidad

    for (int i = 0; i < 200; ++i) {
        double x_value = gRandom->Gaus(3, 0.1);  // Genera valores gaussianos con media 5 y sigma 1
        x.setVal(x_value);  // Asigna el valor generado a 'x'
        data.add(RooArgSet(x));  // Añade el punto al conjunto de datos
        
    }

    // Recta

    RooRealVar X("X", "X", 1, 2);
    
    const int    m = -1;
    const double b = 0.1;

    RooDataSet data2("data2", "Linea", RooArgSet(x));

    for (int i = 0; i<30; ++i){
         double y_value = gRandom->Uniform(2, 3);
         X.setVal(y_value);
         data2.add(RooArgSet(X));
    }


    RooPlot *xframe = x.frame(Title("Gaussian Fit"));
    //data.plotOn(xframe);
    data2.plotOn(xframe);
    
    //gauss.fitTo(data);
    //gauss.plotOn(xframe);

    xframe->Draw();
}






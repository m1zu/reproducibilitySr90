#include "Helpers.hh"

#include <TApplication.h>
#include <TVirtualFitter.h>

double medianLightYield(TH1D* h)
{
  return TMath::Median(h->GetNbinsX(), &(h->GetArray()[1])); //full range
  //return TMath::Median(56, &(h->GetArray()[261])); //shortRange
}

double meanLightYield(TH1D* h)
{
  double mean=0;
  int n = h->GetNbinsX();
  double* channelLightYield = &(h->GetArray()[1]);

  // full range
  for (int i=0; i<n; ++i)
    if ((((i+1)%64)==0 || (i%64)==0))
      ; else mean+=channelLightYield[i];
  mean /= double(n-16);

  /*  // short range
  int count = 0;
  for (int i=0; i<n; ++i)
    if ((i>260) && (i<315)) {
      mean+=channelLightYield[i];
      count++;
    }
  mean /= double(count-1); */

  return mean;
}

double meanLightYieldError(TH1D* h)
{
  double mean = meanLightYield(h);
  int n = h->GetNbinsX();
  double* channelLightYield = &(h->GetArray()[1]);
  double variance = 0;

  // full range
  for (int i=0; i<n; ++i)
    if (((i+1)%64)==0 || (i%64)==0)
      ; else variance+=TMath::Power(mean-channelLightYield[i], 2);
  variance /= double(n-17);
  double rms= TMath::Sqrt(variance)/TMath::Sqrt(n-16);

  /* // short range
  int count = 0;
  for (int i=0; i<n; ++i)
    if ((i>260) && (i<315)) {
      variance+=TMath::Power(mean-channelLightYield[i], 2);
      count++;
    }
  variance /= double(count-1);
  double rms= TMath::Sqrt(variance)/TMath::Sqrt(double(count-1)); */

  return rms;
}

double medianLightYieldError(TH1D* h)
{
  return meanLightYieldError(h)*1.2533;
}

double MAD(TH1D* h)
{
  using namespace TMath;
  Double_t median = medianLightYield(h);
  Double_t* tempArray = new Double_t[h->GetNbinsX()];
  for (int i=0; i<h->GetNbinsX(); ++i) {
    tempArray[i] = Abs( h->GetArray()[i+1] -median);
  }
  return Median(h->GetNbinsX(), tempArray);
}

int main(int argc, char** argv) {
  TApplication application("analysis", &argc, argv);
  //TH1::AddDirectory(false);
  Helpers::setRootStyle();

  /* extracting median, error on median, position from files */

  QString filepath = ("/home/iwanicki/sw/opticalCon/data/4TSAACFIM00127/reproducibility_opticalCon_distance_116/");
  QStringList files = QDir(filepath).entryList(QStringList() << "*posA_Summary*.root");
  const int nFiles = files.count();
  qDebug() << endl << "using" << nFiles << " files ... ";

  TH1D* h_mean = new TH1D("h_mean", "", 18, 9.6, 10.4);
  TH1D* h_median = new TH1D("h_median", "", 14, 9.6, 10.4);

  foreach (QString file, files)
  {
    qDebug() << "\n  ..processing "<< file;

    QString fullPath = filepath + file;
    auto hist = Helpers::extractFromFile<TH1D>(fullPath, "cluster signal value 0 HistogramDisplay").front();
    h_mean->Fill(meanLightYield(hist));
    h_median->Fill(medianLightYield(hist));
  }

  /* distance to SiPM and error calculation */

  Double_t position = 116 + (0.3 + 8.7 - 6.5 + 0.4);
  Double_t posError = TMath::Sqrt((3*0.1*0.1 + 0.01*0.01 + 0.02*0.02)/12.);
  qDebug() << "Distance to SiPM = " << position << " +- " << posError;

  /* draw and fit */

  TCanvas* c1 = new TCanvas("c1", "c1");
  c1->Divide(2,1);
  c1->cd(1);
  gStyle->SetOptStat("nemrou");
  h_mean->SetLineColor(kRed);
  h_mean->GetXaxis()->SetTitle("all channel mean signal / pixel");
  h_mean->Draw();
  c1->cd(2);
  h_median->GetXaxis()->SetTitle("all channel median signal / pixel");
  h_median->SetLineColor(kBlue);
  h_median->Draw();

  /*
  TLegend* l1 = new TLegend(.12,.15,.4,.33);
  l1->AddEntry(g1 ,"fiberMat", "pe");
  l1->AddEntry(g1_con ,"fiberMat with opticalCon", "pe");
  l1->Draw("SAME");
  */

  application.Run();
  return 0;
}

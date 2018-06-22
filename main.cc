#include "Helpers.hh"

#include <TApplication.h>
#include <TVirtualFitter.h>
#include "histcalculator.hh"

int main(int argc, char** argv) {
  TApplication application("analysis", &argc, argv);
  //TH1::AddDirectory(false);
  Helpers::setRootStyle();

  QString filepath = ("/home/iwanicki/sw/gainSr90/data/4TSAACFIM00903/");
  QStringList files = QDir(filepath).entryList(QStringList() << "*posA_Summary*.root");
  const int nFiles = files.count();
  qDebug() << endl << "using" << nFiles << " files ... ";

  const int nChannels = 512;
  TH1D* h_singleChannelGain[nChannels];
  for (int i=0; i<nChannels; ++i) {
      char* histname = new char[10];
      sprintf(histname, "h_%d", i);
      h_singleChannelGain[i]=new TH1D(histname,"",100, 0, 100);
  }
  TH1D* h_example = new TH1D("h_gain_singleChannel", "", 15, 58., 62.);

  foreach (QString file, files)
  {
    qDebug() << "\n  ..processing "<< file;

    QString fullPath = filepath + file;
    auto hist = Helpers::extractFromFile<TH1D>(fullPath, "parameter value 0 HistogramDisplay").front();
    for (int i=1; i<=nChannels; ++i) {
        h_singleChannelGain[i-1]->Fill(hist->GetBinContent(i));
    }
    h_example->Fill ( hist->GetBinContent(17));
  }

  TH1D* h_rms = new TH1D("h_rms_all", "", 25, 0., 1.);
  for (int i=1; i<=nChannels; ++i) {
      h_rms->Fill(h_singleChannelGain[i-1]->GetRMS()/h_singleChannelGain[i-1]->GetMean()*100);
  }
  for (int i=0; i<nChannels; ++i)
      delete h_singleChannelGain[i];

  /* distance to SiPM and error calculation */
  /*
  Double_t position = 116 + (0.3 + 8.7 - 6.5 + 0.4);
  Double_t posError = TMath::Sqrt((3*0.1*0.1 + 0.01*0.01 + 0.02*0.02)/12.);
  qDebug() << "Distance to SiPM = " << position << " +- " << posError;
  */

  /* draw and fit */

  TCanvas* c1 = new TCanvas("c1", "c1");
  gStyle->SetOptStat("nemrou");
  c1->Divide(2,1);
  c1->cd(1);
  h_example->SetLineColor(kRed);
  h_example->GetXaxis()->SetTitle("gain  [pixel]");
  h_example->GetXaxis()->SetTitleOffset(1.30);
  h_example->GetXaxis()->SetTitleSize(25);
  h_example->GetXaxis()->SetLabelOffset(.01);
  h_example->GetXaxis()->SetLabelSize(25);
  h_example->Draw();
  c1->cd(2);
  h_rms->SetLineColor(kRed);
  h_rms->GetXaxis()->SetTitle("RMS / x_{mean}  [%]");
  h_rms->GetXaxis()->SetTitleOffset(1.30);
  h_rms->GetXaxis()->SetTitleSize(25);
  h_rms->GetXaxis()->SetLabelOffset(.01);
  h_rms->GetXaxis()->SetLabelSize(25);
  h_rms->Draw();

  application.Run();
  return 0;
}

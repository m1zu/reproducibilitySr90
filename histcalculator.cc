#include "histcalculator.hh"
#include <TMath.h>

HistCalculator::HistCalculator()
{
}

double HistCalculator::medianLightYield(TH1D* h)
{
  return TMath::Median(h->GetNbinsX(), &(h->GetArray()[1])); //full range
  //return TMath::Median(56, &(h->GetArray()[261])); //shortRange
}

double HistCalculator::meanLightYield(TH1D* h)
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

double HistCalculator::meanLightYieldError(TH1D* h)
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

double HistCalculator::medianLightYieldError(TH1D* h)
{
  return meanLightYieldError(h)*1.2533;
}

double HistCalculator::MAD(TH1D* h)
{
  using namespace TMath;
  Double_t median = medianLightYield(h);
  Double_t* tempArray = new Double_t[h->GetNbinsX()];
  for (int i=0; i<h->GetNbinsX(); ++i) {
    tempArray[i] = Abs( h->GetArray()[i+1] -median);
  }
  return Median(h->GetNbinsX(), tempArray);
}

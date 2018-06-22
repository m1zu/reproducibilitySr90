#ifndef HISTCALCULATOR_H
#define HISTCALCULATOR_H

#include <TH1D.h>

class HistCalculator
{
public:
    HistCalculator();

    double medianLightYield(TH1D* h);
    double meanLightYield(TH1D* h);
    double meanLightYieldError(TH1D* h);
    double medianLightYieldError(TH1D* h);
    double MAD(TH1D* h);
};

#endif // HISTCALCULATOR_H

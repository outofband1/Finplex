#include "utility.h"
#include <math.h>

Utility::Utility(const std::string& name, const std::string& description)
{
    name_ = name;
    description_ = description;
}

void Utility::setCurve(const double& max, const double& saturationAmount, const double& saturationDegree)
{
    // solve max(1-e^saturationAmount*x)=saturationDegree*max)
    max_ = max;
    coefficient_ = log(-saturationDegree + 1);
    coefficient_ /= saturationAmount;
}

double Utility::getUtility(const double& amount) const
{
    double u = max_ * (1.0 - exp(coefficient_ * amount));
    return u;
}

double Utility::getMarginalUtility(const double& amount) const
{
    double mu = max_ * -coefficient_ * exp(coefficient_ * amount);
    return mu;
}

double Utility::getAmountFromMU(const double& targetMU) const
{
    double res = log(-targetMU / (max_ * coefficient_)) / coefficient_;
    return res;
}
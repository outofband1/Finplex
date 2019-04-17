#pragma once
#include <string>

class Utility
{
public:

    Utility(const std::string& name, const std::string& description);

    void setCurve(const double& max, const double& saturationAmount, const double& saturationDegree);

    double getUtility(const double& amount) const;

    double getMarginalUtility(const double& amount) const;

    double getAmountFromMU(const double& targetMU) const;

private:
    double max_;
    double coefficient_;

    std::string name_;
    std::string description_;
};
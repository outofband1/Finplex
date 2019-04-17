#pragma once
#include <map>
#include <memory>
#include "good.h"

class Utility;

class Commodity : public Good
{
public:
    Commodity(const std::string& name, const std::string& description, const FactorsOfProduction& productionInput);

    void setUtility(const std::shared_ptr<Utility>& utility, const double& weight);
    const std::map<std::shared_ptr<Utility>, double>& getUtilities() const;

private:
    std::map<std::shared_ptr<Utility>, double> utilities;
};
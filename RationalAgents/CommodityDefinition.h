#pragma once
#include <string>
#include "FactorsOfProduction.h"
#include <map>

class Utility;

class CommodityDefinition
{
public:
    CommodityDefinition(const std::string& name, const std::string& description, const bool& isActivity);

    const std::string& getName() const;
    const std::string& getDescription() const;

    const bool isActivity() const;

    void setFactorsOfProduction(const FactorsOfProduction& fop);
    const FactorsOfProduction& getFactorsOfProduction() const;

    void setUtility(const std::shared_ptr<Utility>& utility, const double& weight);
    const std::map<std::shared_ptr<Utility>, double>& getUtilities() const;
private:
    std::string name_;
    std::string description_;

    bool isActivity_;

    FactorsOfProduction factorsOfProduction_;
    std::map<std::shared_ptr<Utility>, double> utilities_;
};
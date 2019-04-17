#pragma once
#include <string>
#include "FactorsOfProduction.h"

class Good
{
public:
    Good(const std::string& name, const std::string& description, const FactorsOfProduction& factorsOfProduction);

    const std::string& getName() const;
    const std::string& getDescription() const;

    const FactorsOfProduction& getFactorsOfProduction() const;
private:
    std::string name_;
    std::string description_;

    FactorsOfProduction factorsOfProduction_;
};
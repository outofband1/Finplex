#include "good.h"

Good::Good(const std::string& name, const std::string& description, const FactorsOfProduction& factorsOfProduction) :
    name_(name),
    description_(description),
    factorsOfProduction_(factorsOfProduction)
{

}

const std::string& Good::getName() const
{
    return name_;
}

const std::string& Good::getDescription() const
{
    return description_;
}

const FactorsOfProduction& Good::getFactorsOfProduction() const
{
    return factorsOfProduction_;
}
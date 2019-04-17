#include "Commodity.h"


Commodity::Commodity(const std::string& name, const std::string& description, const FactorsOfProduction& productionInput) :
    Good(name, description, productionInput)
{

}

void Commodity::setUtility(const std::shared_ptr<Utility>& utility, const double& weight)
{
    utilities[utility] = weight;
}

const std::map<std::shared_ptr<Utility>, double>& Commodity::getUtilities() const
{
    return utilities;
}

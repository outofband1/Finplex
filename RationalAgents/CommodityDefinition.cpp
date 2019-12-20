#include "CommodityDefinition.h"

CommodityDefinition::CommodityDefinition(const std::string& name, const std::string& description, const bool& isActivity) :
    name_(name),
    description_(description),
    isActivity_(isActivity)
{

}

const std::string& CommodityDefinition::getName() const
{
    return name_;
}

const std::string& CommodityDefinition::getDescription() const
{
    return description_;
}

const bool CommodityDefinition::isActivity() const
{
    return isActivity_;
}

void CommodityDefinition::setFactorsOfProduction(const FactorsOfProduction& fop)
{
    factorsOfProduction_ = fop;
}

const FactorsOfProduction& CommodityDefinition::getFactorsOfProduction() const
{
    return factorsOfProduction_;
}

void CommodityDefinition::setUtility(const std::shared_ptr<Utility>& utility, const double& weight)
{
    utilities_[utility] = weight;
}

const std::map<std::shared_ptr<Utility>, double>& CommodityDefinition::getUtilities() const
{
    return utilities_;
}
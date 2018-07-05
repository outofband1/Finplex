#include "Commodity.h"

Commodity::Commodity(const std::string name) :
    name_(name)
{

}

void Commodity::setDescription(const std::string description)
{
    description_ = description;
}

void Commodity::addUtility(const std::shared_ptr<Utility>& utility, const float& amount)
{
    utilities_[utility] = amount;
}

const std::string& Commodity::getName() const
{
    return name_;
}

const std::string& Commodity::getDescription() const
{
    return description_;
}

const std::map<std::shared_ptr<Utility>, float>& Commodity::getUtilities() const
{
    return utilities_;
}
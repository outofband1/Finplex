#include "Commodity.h"

Commodity::Commodity(const std::string name) :
    name_(name)
{

}

void Commodity::setDescription(const std::string description)
{
    description_ = description;
}

const std::string& Commodity::getName() const
{
    return name_;
}

const std::string& Commodity::getDescription() const
{
    return description_;
}
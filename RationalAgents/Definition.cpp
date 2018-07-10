#include "Definition.h"

Definition::Definition(const std::string name) :
    name_(name)
{

}

void Definition::setName(const std::string& name)
{
    name_ = name;
}

void Definition::setDescription(const std::string description)
{
    description_ = description;
}

const std::string& Definition::getName() const
{
    return name_;
}

const std::string& Definition::getDescription() const
{
    return description_;
}

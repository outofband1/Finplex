#include "Entity.h"

Entity::Entity(const std::string& name) :
    name_(name),
    currency_(0)
{

}

const std::string& Entity::getName() const
{
    return name_;
}

Inventory& Entity::getInventory()
{
    return inventory_;
}

const double& Entity::getCurrency()
{
    return currency_;
}

void Entity::addCurrency(const double& amount)
{
    currency_ += amount;
}

void Entity::removeCurrency(const double& amount)
{
    if (amount < currency_)
    {
        currency_ -= amount;
    }
    else
    {
        throw;
    }
}

void Entity::transferCurrency(std::shared_ptr<Entity>& target, const double & amount)
{
    removeCurrency(amount);
    target->addCurrency(amount);
}
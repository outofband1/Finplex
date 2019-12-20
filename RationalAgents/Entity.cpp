#include "Entity.h"
#include "Inventory.h"

Entity::Entity(const std::string& name) :
    name_(name)
{

}

const std::string& Entity::getName() const
{
    return name_;
}

void Entity::registerOwnership(const TradableGood& tradableGood, std::shared_ptr<Inventory> inventory)
{
    ownedGoods_[tradableGood].insert(inventory);
}

void Entity::unregisterOwnership(const TradableGood& tradableGood, std::shared_ptr<Inventory> inventory)
{
    auto& it = ownedGoods_.find(tradableGood);
    it->second.erase(inventory);

    if (it->second.empty())
    {
        ownedGoods_.erase(it);
    }
}
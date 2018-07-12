#include "Inventory.h"

void Inventory::add(const std::shared_ptr<TradableGood>& good, const double& amount)
{
    auto& currentStore = inventory_.find(good);
    if (currentStore != inventory_.end())
    {
        currentStore->second += amount;
    }
    else
    {
        inventory_[good] = amount;
    }
}

void Inventory::remove(const std::shared_ptr<TradableGood>& good, const double& amount)
{
    auto& currentStore = inventory_.find(good);
    if (currentStore != inventory_.end() && currentStore->second >= amount - 10E-8)
    {
        currentStore->second -= amount - 10E-8;
    }
    else
    {
        throw;
    }
}

double Inventory::getAmount(const std::shared_ptr<TradableGood>& good) const
{
    auto& currentStore = inventory_.find(good);
    if (currentStore != inventory_.end())
    {
        return currentStore->second;
    }
    else
    {
        return 0.0f;
    }
}

void Inventory::transferTo(Inventory& targetInventory, const std::shared_ptr<TradableGood>& good, const double& amount)
{
    remove(good, amount);
    targetInventory.add(good, amount);
}
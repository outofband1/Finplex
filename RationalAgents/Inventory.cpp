#include "Inventory.h"

#include "CommodityInstance.h"
#include "Exceptions.h"
#include "TradableGood.h"
#include "CommodityDefinition.h"

Inventory::Inventory()
{

}

void Inventory::create(const TradableGood& tradableGood, const double amount)
{
    auto& item = items_.find(tradableGood);

    if (item != items_.end())
    {
        item->second += amount;
    }
    else
    {
        items_[tradableGood] = amount;
    }

    auto& owner = tradableGood.getOwner();
    owner->registerOwnership(tradableGood, shared_from_this());
}

void Inventory::transferTo(const TradableGood& tradableGood, Inventory& targetInventory, const std::shared_ptr<Entity> targetOwner, const double amount)
{
    try
    {
        targetInventory.create(TradableGood(tradableGood.getCommodity(), targetOwner), amount);
        destroy(tradableGood, amount);
    }
    catch (Exceptions::OneMessage& e)
    {
        std::cout << e.getMessage();
    }
}

void Inventory::transferAllTo(Inventory& target)
{
    try
    {
        for (auto& item : items_)
        {
            target.create(item.first, item.second);
        }


        for (auto& item : items_)
        {
            auto& owner = item.first.getOwner();
            owner->unregisterOwnership(item.first, shared_from_this());
        }

        items_.clear();
    }
    catch (Exceptions::OneMessage& e)
    {
        std::cout << e.getMessage();
    }
}

double Inventory::getAmount(const TradableGood& tradableGood) const
{
    auto& item = items_.find(tradableGood);

    if (item != items_.end())
    {
        return item->second;
    }
    else
    {
        return 0.0;
    }
}

double Inventory::getAmount(const std::shared_ptr<CommodityDefinition>& commodity) const
{
    double amount = 0.0;
    for (auto& item : items_)
    {
        if (item.first.getCommodity()->getCommodityDefinition() == commodity)
        {
            amount += item.second;
        }
    }

    return amount;
}

const std::map<TradableGood, double>& Inventory::getItems()
{
    return items_;
}

void Inventory::destroy(const TradableGood& tradableGood, const double amount)
{
    auto& item = items_.find(tradableGood);

    if (item != items_.end())
    {
        item->second -= amount;

        if (item->second < -10E-6)
        {
            PITCH(Exceptions::OneMessage, "Commodity (amount) for destruction not found in inventory");
        }
        else if (item->second < 10E-6)
        {
            auto& owner = tradableGood.getOwner();
            owner->unregisterOwnership(tradableGood, shared_from_this());

            item->second = 0.0;
            items_.erase(item);


        }
    }
    else
    {
        PITCH(Exceptions::OneMessage, "Commodity for destruction not found in inventory");
    }
}

void Inventory::print()
{
    for (auto& item : items_)
    {
        std::cout << item.first.getCommodity()->getName() << "    : " << item.second << std::endl;
    }
}
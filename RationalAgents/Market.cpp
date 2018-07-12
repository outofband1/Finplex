#include "Market.h"
#include "Entity.h"
#include "Commodity.h"
#include "TradableGood.h"
#include <iostream>

void Market::setSellingPrice(std::shared_ptr<TradableGood> good, const double& newPrice)
{
    prices_[good] = newPrice;
}

const double& Market::getSellingPrice(std::shared_ptr<TradableGood> good) const
{
    auto& price = prices_.find(good);
    if (price != prices_.end())
    {
        return price->second;
    }
    else
    {
        throw;
    }
}

void Market::sell(const std::shared_ptr<TradableGood>& good, const std::shared_ptr<Entity>& buyer, const double& amount)
{
    std::shared_ptr<TradableGood> newGood = std::make_shared<TradableGood>(*good);
    newGood->transferOwnership(buyer);

    buyer->getInventory().add(newGood, amount);
    remove(good, amount);

    double currency = amount * prices_[good];
    buyer->removeCurrency(currency);
    good->getOwner()->addCurrency(currency);
    //buyer->transferCurrency(good->getOwner(), currency);
}

void Market::print()
{
    for (auto& good : inventory_)
    {
        double price = -1;
        auto& priceIt = prices_.find(good.first);
        if (priceIt != prices_.end())
        {
            price = priceIt->second;
        }

        std::cout << good.first->getCommodity()->getName() << ", Amount: " << good.second << ", Price: " << price << std::endl;

    }
}
#pragma once
#include <string>
#include <set>
#include <memory>
#include <vector>
#include "CommodityInstance.h"
#include "Inventory.h"
#include <unordered_map>
#include "TradableGood.h"

class Facility;
class Factory;
class TradableGood;

class Entity : public std::enable_shared_from_this<Entity>
{
public:
    Entity(const std::string& name);

    const std::string& getName() const;

    double money_ = 0.0;

    virtual void receiveClearingInformation(const std::shared_ptr<GenericCommodity>& good, const double amountSold, const double price)
    {
        throw;
    }

    virtual double getMarketPrice(const std::shared_ptr<GenericCommodity>& good)
    {
        throw;
    }

    void registerOwnership(const TradableGood& tradableGood, std::shared_ptr<Inventory> inventory);

    void unregisterOwnership(const TradableGood& tradableGood, std::shared_ptr<Inventory> inventory);

    virtual void newRound() = 0;

protected:

    std::map<TradableGood, std::set<std::shared_ptr<Inventory>>> ownedGoods_;
private:
    std::string name_;

};


#pragma once

#include <memory>
#include <map>

class Entity;
class TradableGood;
class CommodityDefinition;

class Inventory : public std::enable_shared_from_this<Inventory>
{
public:
    Inventory();

    void create(const TradableGood & tradableGood, const double amount);
    void destroy(const TradableGood & tradableGood, const double amount);

    void transferTo(const TradableGood & tradableGood, Inventory & targetInventory, const std::shared_ptr<Entity> targetOwner, const double amount);

    void transferAllTo(Inventory & target);

    double getAmount(const TradableGood & tradableGood) const;

    double getAmount(const std::shared_ptr<CommodityDefinition>& commodity) const;

    const std::map<TradableGood, double>& getItems();

    void print();
private:


    // (owner, commodity)-> amount
    std::map<TradableGood, double> items_;
};
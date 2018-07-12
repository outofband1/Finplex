#pragma once

#include "memory"
#include <map>

class TradableGood;

class Inventory
{
public:
    void add(const std::shared_ptr<TradableGood>& good, const double& amount);
    void remove(const std::shared_ptr<TradableGood>& good, const double& amount);

    double getAmount(const std::shared_ptr<TradableGood>& good) const;

    void transferTo(Inventory& targetInventory, const std::shared_ptr<TradableGood>& good, const double& amount);
    std::map<std::shared_ptr<TradableGood>, double> inventory_;
protected:
    //std::map<std::shared_ptr<TradableGood>, double> inventory_;
};

#pragma once
#include <memory>
#include <map>
#include "Inventory.h"

class Entity;
class TradableGood;

class Market : public Inventory
{
public:
    void setSellingPrice(std::shared_ptr<TradableGood> good, const double & newPrice);
    const double& getSellingPrice(std::shared_ptr<TradableGood> good) const;

    void sell(const std::shared_ptr<TradableGood>& good, const std::shared_ptr<Entity>& buyer, const double& amount);

    void print();



    std::map< std::shared_ptr<TradableGood>, double> prices_;
private:
    //std::map< std::shared_ptr<TradableGood>, double> prices_;


};
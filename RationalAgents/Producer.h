#pragma once

#include <memory>

#include "Inventory.h"
#include "Entity.h"

class PurchaseSolver;
class Commodity;
class Market;

class Producer : public Entity
{
public:
    Producer(const std::string& name, const std::shared_ptr<Commodity>& product);

    void optimizeProductionAndPrice(PurchaseSolver& purchaseSolver, const Market& market, double blah);
    void goToMarket(Market &market);

    const double& getCapacity() const;

    const std::shared_ptr<TradableGood>& getProduct() const;

    double plannedProductionAmount_;
    double plannedPrice_;
    double plannedWage_;
private:

    double capacity_;

    std::shared_ptr<TradableGood> product_;
};
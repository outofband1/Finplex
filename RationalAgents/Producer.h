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

    double optimizeProductionAndPrice(PurchaseSolver& purchaseSolver, const Market& market);
    void goToMarket(Market & market);

    const double& getCapacity() const;

    const std::shared_ptr<TradableGood>& getProduct() const;

    double moneys = 100.0;
private:
    double plannedProductionAmount_;
    double plannedPrice_;

    double capacity_;

    std::shared_ptr<TradableGood> product_;
};
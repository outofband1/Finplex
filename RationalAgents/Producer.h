#pragma once

#include <memory>

#include "Inventory.h"

class Commodity;
class PurchaseSolver;

class Producer
{
public:
    Producer(const std::shared_ptr<Commodity>& product);

    double planProduction(PurchaseSolver& purchaseSolver, const double& previousAmount, const double& previousPrice);

    const double& getCapacity() const;

    const std::shared_ptr<Commodity>& getProduct() const;

    double moneys = 100.0;
private:
    double plannedProductionAmount_;
    double plannedPrice_;

    double capacity_;

    double prevPrice_;



    const std::shared_ptr<Commodity> product_;
    Inventory inventory_;
    Inventory market_;
};
#include "Producer.h"
#include "PurchaseSolver.h"
#include "Commodity.h"
#include <iostream>

Producer::Producer(const std::shared_ptr<Commodity>& product) :
    product_(product)
{
    capacity_ = 1.5;
    plannedProductionAmount_ = capacity_ / 2.0;
    plannedPrice_ = 1.0;
}

const double& Producer::getCapacity() const
{
    return capacity_;
}

const std::shared_ptr<Commodity>& Producer::getProduct() const
{
    return product_;
}

double Producer::planProduction(PurchaseSolver& purchaseSolver, const double& previousAmount, const double& previousPrice)
{
    double produceMax = 1.5;
    double price = 0.8;

    Inventory summedInventories;
    double summedMoney = 0.0;
    std::map<std::shared_ptr<Commodity>, double> commodityPurchases;
    std::map<std::shared_ptr<Activity>, double> activityPurchases;



    purchaseSolver.setPriceAndAmount(product_, plannedPrice_, plannedProductionAmount_);
    purchaseSolver.OptimizeTimeAndPurchases(24, 0.0, summedInventories, commodityPurchases, activityPurchases);
    double amountSoldOrg = commodityPurchases.find(product_)->second;
    double profitOrg = amountSoldOrg * plannedPrice_ - plannedProductionAmount_ - 1;


    purchaseSolver.setPriceAndAmount(product_, plannedPrice_ * 1.01, plannedProductionAmount_);
    purchaseSolver.OptimizeTimeAndPurchases(24, 0.0, summedInventories, commodityPurchases, activityPurchases);
    double amountSoldPriceGrad = commodityPurchases.find(product_)->second;
    double priceGrad = amountSoldPriceGrad * plannedPrice_ * 1.01 - plannedProductionAmount_ - 1 - profitOrg;

    purchaseSolver.setPriceAndAmount(product_, plannedPrice_, plannedProductionAmount_ * 1.01);
    purchaseSolver.OptimizeTimeAndPurchases(24, 0.0, summedInventories, commodityPurchases, activityPurchases);
    double amountSoldProducedGrad = commodityPurchases.find(product_)->second;
    double producedGrad = amountSoldProducedGrad * plannedPrice_ - plannedProductionAmount_ * 1.01 - 1 - profitOrg;

    double len = sqrt(priceGrad * priceGrad + producedGrad * producedGrad);
    if (len > 0.0)
    {
        priceGrad /= len;
        producedGrad /= len;

        plannedPrice_ += 0.01 * priceGrad;
        plannedProductionAmount_ += 0.01 * producedGrad;
        if (plannedProductionAmount_ > capacity_)
        {
            plannedProductionAmount_ = capacity_;
        }
    }
    else
    {
        plannedPrice_ *= 0.99;
    }


    purchaseSolver.setPriceAndAmount(product_, plannedPrice_, plannedProductionAmount_);

    return plannedPrice_;
}

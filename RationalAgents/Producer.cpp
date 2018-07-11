#include "Producer.h"
#include "PurchaseSolver.h"
#include "Commodity.h"
#include <iostream>

Producer::Producer(const std::shared_ptr<Commodity>& product) :
    product_(product)
{
    capacity_ = 2.0;
    plannedProductionAmount_ = capacity_ / 2.0;
    plannedPrice_ = 0.8;
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
    const double stepsizePrice = 0.01;
    const double stepsizeProd = 0.01;

    double cost = 1.0;
    double minPrice = 1.1;

    Inventory summedInventories;
    double summedMoney = 0.0;
    std::map<std::shared_ptr<Commodity>, double> commodityPurchases;
    std::map<std::shared_ptr<Activity>, double> activityPurchases;



    //std::cout << plannedProductionAmount_ << "   ,   " << plannedPrice_ << std::endl;
    purchaseSolver.setPriceAndAmount(product_, plannedPrice_, plannedProductionAmount_);
    purchaseSolver.OptimizeTimeAndPurchases(24, 0.0, summedInventories, commodityPurchases, activityPurchases);
    double amountSoldOrg = commodityPurchases.find(product_)->second;
    double profitOrg = amountSoldOrg * plannedPrice_ - plannedProductionAmount_ - (capacity_ - amountSoldOrg) * plannedPrice_ ;

    if (plannedPrice_ * (1 + stepsizePrice) >= minPrice && plannedProductionAmount_ > 0)
    {
        //std::cout << plannedProductionAmount_ << "   ,   " << plannedPrice_* (1 + stepsizePrice) << std::endl;
        purchaseSolver.setPriceAndAmount(product_, plannedPrice_ * (1 + stepsizePrice), plannedProductionAmount_);
        purchaseSolver.OptimizeTimeAndPurchases(24, 0.0, summedInventories, commodityPurchases, activityPurchases);
    }
    double amountSoldPriceGrad = commodityPurchases.find(product_)->second;
    double priceGrad = amountSoldPriceGrad * plannedPrice_ * (1 + stepsizePrice) - plannedProductionAmount_ - (capacity_ - amountSoldPriceGrad) * (1 + stepsizePrice) * plannedPrice_ - profitOrg;

    if (plannedProductionAmount_ * (1 + stepsizeProd) > 0 && plannedPrice_ >= minPrice)
    {
        //std::cout << plannedProductionAmount_ * (1 + stepsizeProd) << "   ,   " << plannedPrice_ << std::endl;
        purchaseSolver.setPriceAndAmount(product_, plannedPrice_, plannedProductionAmount_ * (1 + stepsizeProd));
        purchaseSolver.OptimizeTimeAndPurchases(24, 0.0, summedInventories, commodityPurchases, activityPurchases);
    }
    double amountSoldProducedGrad = commodityPurchases.find(product_)->second;
    double producedGrad = amountSoldProducedGrad * plannedPrice_ - plannedProductionAmount_ * (1 + stepsizeProd) - (capacity_ - amountSoldProducedGrad) * plannedPrice_ - profitOrg;

    double len = sqrt(priceGrad * priceGrad + producedGrad * producedGrad);
    if (len > 0.0)
    {
        priceGrad /= len;
        producedGrad /= len;

        plannedPrice_ += stepsizePrice * priceGrad;
        if (plannedPrice_ < minPrice)
        {
            plannedPrice_ = minPrice;
        }

        plannedProductionAmount_ += stepsizeProd * producedGrad;
        if (plannedProductionAmount_ > capacity_)
        {
            plannedProductionAmount_ = capacity_;
        }
        else if (plannedProductionAmount_ < 0)
        {
            plannedProductionAmount_ = 0;
        }
    }
    else if (plannedPrice_ * (1 - stepsizePrice) >= minPrice)
    {
        plannedPrice_ *= 1 - stepsizePrice;
    }
    else
    {
        plannedPrice_ = minPrice;
    }


    if (plannedProductionAmount_ > 0 && plannedPrice_ >= minPrice)
    {
        purchaseSolver.setPriceAndAmount(product_, plannedPrice_, plannedProductionAmount_);
        std::cout << "Producing: " << plannedProductionAmount_ << " " << product_->getName() << " at price: " << plannedPrice_ << " for profit: " << profitOrg + (capacity_ - amountSoldOrg)*plannedPrice_ << std::endl;

    }
    else
    {
        purchaseSolver.removeUtilitySource(product_);
    }

    return plannedPrice_;
}

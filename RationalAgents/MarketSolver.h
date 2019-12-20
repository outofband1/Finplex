#pragma once
#include <memory>
#include <map>
#include "Utility.h"
#include "CommodityInstance.h"
#include <vector>

class ConsumerMarket;
class CommodityDefinition;
class Entity;
class TradableGood;

namespace market_solver
{
    class Result
    {
    public:
        void setAmountSold(double amountSold);
        double getAmountSold() const;
        void setPrice(double price);
        double getPrice() const;

    private:
        double amountSold_;
        double price_;
    };

    class Solver
    {
    public:
        void clear();

        void registerProduct(const TradableGood& tradableGood, const double & capacity, const double & minPrice);

        void findPurchases(double scale, double budget, bool pricesLocked, std::map<TradableGood, Result>& results);

    private:
        struct PriceAndAmount
        {
            double minPrice_;
            double price_;
            double amount_;
            double mu_;
            double capacity_;
            bool active_;
            double effectivePrice_;
        };


        struct CommodityBalance
        {
            double capacity;
            double needed;
            double price;
            double priceAdd;
        };

        struct ProductConstraint
        {
            double minimumPrice_;
            double maximumAmount_;
        };

        void updateNeeded(std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance>& balance, std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance>::iterator& comBalance, double add);

        void refreshMinimumPrices(std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance>& balance,
                                  std::map<TradableGood, PriceAndAmount>& pandas, const double& normalizationFactor);

        void refreshCommodityInput(std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance>& balance,
                                   const std::map<TradableGood, PriceAndAmount>& pandas);

        void refreshUtilities(double scale, std::map<TradableGood, PriceAndAmount>& pandas);

        void calculateUtilityAmounts(double scale, std::map<TradableGood, PriceAndAmount>& pandas);

        double getMarginalUtility(const std::shared_ptr<GenericCommodity>& product) const;

        std::map<TradableGood, ProductConstraint> constraints_;

        std::map <std::shared_ptr<Utility>, double> utilityAmounts_;
    };

    void printResults(const std::map<TradableGood, Result>& results);
}
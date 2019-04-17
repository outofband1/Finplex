#pragma once
#include <memory>
#include <map>
#include "CommodityInstance.h"
#include "Utility.h"

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
        void registerProduct(const std::shared_ptr<CommodityInstance> & product, const double & capacity, const double & minPrice);

        void findPurchases(double scale, double budget, bool pricesLocked, std::map<std::shared_ptr<CommodityInstance>, Result>& results);

    private:
        struct PriceAndAmount
        {
            double minPrice_;
            double price_;
            double amount_;
            double mu_;
            double capacity_;
            bool active_;
        };

        struct ProductConstraint
        {
            double minimumPrice_;
            double maximumAmount_;
        };

        void refreshUtilities(double scale, std::map<std::shared_ptr<CommodityInstance>, PriceAndAmount>& pandas);

        void calculateUtilityAmounts(double scale, std::map<std::shared_ptr<CommodityInstance>, PriceAndAmount>& pandas);

        double getMarginalUtility(const std::shared_ptr<CommodityInstance>& product) const;

        std::map<std::shared_ptr<CommodityInstance>, ProductConstraint> constraints_;

        std::map <std::shared_ptr<Utility>, double> utilityAmounts_;
    };

    void printResults(const std::map<std::shared_ptr<CommodityInstance>, Result>& results);
}
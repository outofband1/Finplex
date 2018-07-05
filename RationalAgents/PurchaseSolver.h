#pragma once
#include <vector>
#include <memory>
#include "Simplex.h"
#include <map>

class Commodity;
class Utility;

class PurchaseSolver
{
public:
    PurchaseSolver();
    void registerCommodity(const std::shared_ptr<Commodity>& commodity);
    void registerUtility(const std::shared_ptr<Utility>& utility);

    void setPriceAndAmount(const std::shared_ptr<Commodity>& commodity, const float& price, const float& amount);

    void clearAmountsAndPrices();

	void OptimizeTimeAndPurchases(const float& money, std::map<std::shared_ptr<Commodity>, float>& purchases) const;

private:
    struct PriceAndAmount
    {
        float price_;
        float amount_;
    };

    std::map<std::shared_ptr<Commodity>, PriceAndAmount> amountsAndPrices_;
    std::vector<std::shared_ptr<Commodity>> commodities_;
    std::vector<std::shared_ptr<Utility>> utilities_;
};
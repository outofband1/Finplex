#pragma once
#include <vector>
#include <memory>
#include <map>

class Commodity;
class Activity;
class UtilitySource;
class Utility;
class Inventory;

class PurchaseSolver
{
public:
    PurchaseSolver();
    void registerUtilitySource(const std::shared_ptr<Commodity>& commodity);
    void registerUtilitySource(const std::shared_ptr<Activity>& activity);

    void registerUtility(const std::shared_ptr<Utility>& utility);

    void setPriceAndAmount(const std::shared_ptr<Commodity>& utilitySource, const double& price, const double& amount);
    void setPriceAndAmount(const std::shared_ptr<Activity>& utilitySource, const double& price, const double& amount);

    void clearAmountsAndPrices();

    void removeUtilitySource(const std::shared_ptr<Commodity>& commodity);
    void removeUtilitySource(const std::shared_ptr<Activity>& activity);

    void OptimizeTimeAndPurchases(const double& time, const double& savings, const Inventory& inventory, std::map<std::shared_ptr<Commodity>, double>& commodityPurchases, std::map<std::shared_ptr<Activity>, double>& activityPurchases) const;

    double OptimizeTimeAndPurchases2(const double& time, const double& savings, const bool& laborConstraints, const Inventory& inventory, std::map<std::shared_ptr<Commodity>, double>& commodityPurchases, std::map<std::shared_ptr<Activity>, double>& activityPurchases) const;

private:
    struct PriceAndAmount
    {
        double price_;
        double amount_;
    };

    std::map<std::shared_ptr<Commodity>, PriceAndAmount> commodityAmountsAndPrices_;
    std::map<std::shared_ptr<Activity>, PriceAndAmount> activityAmountsAndPrices_;
    //std::vector<std::shared_ptr<Commodity>> commodities_;
    //std::vector<std::shared_ptr<Activity>> activities_;
    std::vector<std::shared_ptr<Utility>> utilities_;
};
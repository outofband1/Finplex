#pragma once
#include "Entity.h"
#include <vector>
#include <memory>
#include "Agent.h"

class Factory;
class SimulationLocal;
class ConsumerMarket;

class Company : public Entity
{
public:
    Company(const std::string& name);

    void submitGoodsForMarketAnalysis(SimulationLocal& sim);

    virtual void receiveClearingInformation(const std::shared_ptr<GenericCommodity>& good, const double amountSold, const double price);

    void produce(ConsumerMarket& market);

    void setMarketPrices(ConsumerMarket& market);

    void purchaseFromMarket(ConsumerMarket& market);

    void setOwnerShip(std::shared_ptr<Factory> factory);

    void removeOwnerShip(std::shared_ptr<Factory> factory);

    virtual double getMarketPrice(const std::shared_ptr<GenericCommodity>& good);

    void newRound();
private:
    struct ClearingInfo
    {
        double amount;
        double price;
    };

    struct History
    {
        double price;
        double capacity;
    };

    // ownerships
    std::set<std::shared_ptr<Factory>> factories_;

    // clearing info by good
    std::map<std::shared_ptr<GenericCommodity>, ClearingInfo> clearingByGood_;

    // history by good
    std::map<std::shared_ptr<GenericCommodity>, History> historyByGood_;
};
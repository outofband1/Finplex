#pragma once
#include <map>
#include <memory>
#include "TradableGood.h"
#include "TransportJob.h"

class Inventory;
class Entity;

class ConsumerMarket
{
public:

    struct Item
    {
        Item(const TradableGood& tradableGood, const double& price, const double& amount);

        TradableGood tradableGood_;
        double price_;
        double amount_;
    };

    ConsumerMarket();

    Inventory& getInventory();

    std::shared_ptr<Inventory> getPickupInventory();

    const std::vector<Item>& getItems() const;

    void buyGeneric(const std::shared_ptr<CommodityDefinition> good,
                    const double amount,
                    const std::shared_ptr<Entity> buyer,
                    std::map<TradableGood, double>& purchases);


    void makeTransaction(const TradableGood& tradableGood,
                         const std::shared_ptr<Entity> buyer,
                         const double amount,
                         const double price);

    void updatePrices();

    void addTransportJob(const TradableGood& tradableGood, const double amount, const std::shared_ptr<Inventory> source, const std::shared_ptr<Inventory> destination);

    const std::map<std::shared_ptr<TransportJob>, bool>& getTransportJobs() const;

    void acceptTransportJob(const std::shared_ptr<TransportJob> transportJob);

    // called at beginning of each trading round (day?)
    void newRound();

private:
    std::shared_ptr<Inventory> inventory_;

    std::shared_ptr<Inventory> pickupInventory_;

    std::vector<Item> items_;

    std::map<std::shared_ptr<TransportJob>, bool> transportJobs_;
};
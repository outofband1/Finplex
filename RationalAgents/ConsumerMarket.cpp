#include "ConsumerMarket.h"
#include "Entity.h"
#include "CommodityDefinition.h"
#include <iostream>
#include "Inventory.h"

ConsumerMarket::Item::Item(const TradableGood& tradableGood, const double& price, const double& amount) :
    tradableGood_(tradableGood),
    price_(price),
    amount_(amount)
{

}

ConsumerMarket::ConsumerMarket()
{
    pickupInventory_ = std::make_shared<Inventory>();
    inventory_ = std::make_shared<Inventory>();
}

Inventory& ConsumerMarket::getInventory()
{
    return *inventory_;
}

std::shared_ptr<Inventory> ConsumerMarket::getPickupInventory()
{
    return pickupInventory_;
}

const std::vector<ConsumerMarket::Item>& ConsumerMarket::getItems() const
{
    return items_;
}

void ConsumerMarket::buyGeneric(const std::shared_ptr<CommodityDefinition> good,
                                const double amount,
                                const std::shared_ptr<Entity> buyer,
                                std::map<TradableGood, double>& purchases)
{
    double boughtLastRound = -1;
    double bought = 0.0;

    std::vector <ConsumerMarket::Item> potentialItems;

    for (auto& item : items_)
    {
        if (item.tradableGood_.getCommodity()->getCommodityDefinition() == good && item.amount_ > 10E-6)
        {
            potentialItems.push_back(item);
        }
    }

    double amountLeft = amount;

    while (amountLeft > 10E-6 && potentialItems.size() > 0)
    {
        int remainingPotentialSellers = static_cast<int>(potentialItems.size());
        double amountperSeller = amountLeft / remainingPotentialSellers;

        for (std::vector <ConsumerMarket::Item>::iterator item = potentialItems.begin(); item != potentialItems.end(); item++)
        {
            double amountInInv = item->amount_;
            const std::shared_ptr<Entity> owner = item->tradableGood_.getOwner();
            double priceVal = item->price_;
            double actualAmount = amountperSeller;

            bool itemDone = false;
            if (amountInInv <= amountperSeller)
            {
                actualAmount = amountInInv;
                itemDone = true;
            }

            amountLeft -= actualAmount;

            auto& existingPurchase = purchases.find(TradableGood(item->tradableGood_.getCommodity(), buyer));
            if (existingPurchase != purchases.end())
            {
                existingPurchase->second += actualAmount;
            }
            else
            {
                purchases.insert(std::make_pair(TradableGood(item->tradableGood_.getCommodity(), buyer), actualAmount));
            }

            makeTransaction(item->tradableGood_, buyer, actualAmount, priceVal);

            if (itemDone)
            {
                potentialItems.erase(item);
                break;
            }
        }
    }




    /*do
    {
        boughtLastRound = bought;
        for (auto& item : items_)
        {
            if (item.tradableGood_.getCommodity()->getCommodityDefinition() == good)
            {
                double amountInInv = item.amount_;

                if (amountInInv < 10E-6)
                {
                    continue;
                }

                const std::shared_ptr<Entity> owner = item.tradableGood_.getOwner();

                double priceVal = item.price_;

                double actualAmount = 0.0;
                if (amountInInv <= amount)
                {
                    actualAmount = amountInInv;

                }
                else
                {
                    actualAmount = amount;
                }

                auto& existingPurchase = purchases.find(TradableGood(item.tradableGood_.getCommodity(), buyer));

                if (existingPurchase != purchases.end())
                {
                    existingPurchase->second += actualAmount;
                }
                else
                {
                    purchases.insert(std::make_pair(TradableGood(item.tradableGood_.getCommodity(), buyer), actualAmount));
                }

                makeTransaction(item.tradableGood_, buyer, actualAmount, priceVal);
                bought += actualAmount;

                break;
            }
        }
    }
    while (bought < amount - 10E-6 && bought > boughtLastRound + 10E-6);*/
}


void ConsumerMarket::makeTransaction(const TradableGood& tradableGood,
                                     const std::shared_ptr<Entity> buyer,
                                     const double amount,
                                     const double price)
{
    buyer->money_ -= amount * price;
    // call before transfer or it could get destroyed
    tradableGood.getOwner()->money_ += amount * price;
    std::cout << buyer->getName() << " bought " << amount << " " << tradableGood.getCommodity()->getName() << " for " << price << " each." << std::endl;

    inventory_->transferTo(tradableGood, *pickupInventory_, buyer, amount);

    for (auto& item : items_)
    {
        if (item.tradableGood_ == tradableGood)
        {
            item.amount_ -= amount;
            break;
        }
    }
}

void ConsumerMarket::updatePrices()
{

    for (auto& invItem : inventory_->getItems())
    {
        double price = invItem.first.getOwner()->getMarketPrice(invItem.first.getCommodity());

        Item newItem(invItem.first, price, invItem.second);
        items_.push_back(newItem);
    }
}

void ConsumerMarket::addTransportJob(const TradableGood& tradableGood, const double amount, const std::shared_ptr<Inventory> source, const std::shared_ptr<Inventory> destination)
{
    transportJobs_.insert(std::make_pair(std::make_shared<TransportJob>(tradableGood, amount, source, destination), false));
}

const std::map<std::shared_ptr<TransportJob>, bool>& ConsumerMarket::getTransportJobs() const
{
    return transportJobs_;
}

void ConsumerMarket::acceptTransportJob(const std::shared_ptr<TransportJob> transportJob)
{
    transportJobs_[transportJob] = true;
}

void ConsumerMarket::newRound()
{
    transportJobs_.clear();
    items_.clear(); // this assumes a new trading round
}
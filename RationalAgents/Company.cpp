#include "Company.h"
#include <memory>
#include "SimulationLocal.h"
#include "Factory.h"
#include "FactorsOfProduction.h"

Company::Company(const std::string& name) :
    Entity(name)
{

}

void Company::setOwnerShip(std::shared_ptr<Factory> factory)
{
    factories_.insert(factory);

    auto& producedGood = factory->getCommodityInstance();
}

void Company::removeOwnerShip(std::shared_ptr<Factory> factory)
{
    auto& producedGood = factory->getCommodityInstance();

    if (factories_.find(factory) != factories_.end())
    {
        factories_.erase(factory);
    }
}

void Company::submitGoodsForMarketAnalysis(SimulationLocal& sim)
{
    for (auto& factory : factories_)
    {
        auto& good = factory->getCommodityInstance();
        double currentAmount = sim.getMarket().getInventory().getAmount(TradableGood(good, shared_from_this()));

        // adding current amount increases capacity and capacity of factories depending on this good.
        // this leads to oscillations because of lags between production and next round prices.
        // code below adds a simple moving average to dampen oscillations
        // the 9 to 10 factor put emphasis on previous iterations
        double maxCapacity = factory->getCapacity() + currentAmount;

        double capacity = 0.0;

        auto& previous = historyByGood_.find(good);

        if (previous != historyByGood_.end())
        {
            capacity = (maxCapacity + 9 * previous->second.capacity) / 10.0;
        }
        else
        {
            capacity = maxCapacity;
        }

        if (capacity > maxCapacity)
        {
            capacity = maxCapacity;
        }

        historyByGood_[good].capacity = capacity;

        sim.acceptGoodForMarketAnalysis(TradableGood(good, shared_from_this()), capacity);
    }
}

void Company::receiveClearingInformation(const std::shared_ptr<GenericCommodity>& good, const double amountSold, const double price)
{
    ClearingInfo info;
    info.amount = amountSold;
    info.price = price;

    clearingByGood_[good] = info;
}

void Company::produce(ConsumerMarket& market)
{
    for (auto& factory : factories_)
    {
        auto& commodity = factory->getCommodityInstance();

        auto& clearingInfo = clearingByGood_.find(commodity);

        if (clearingInfo != clearingByGood_.end())
        {
            TradableGood tmpTradable(commodity, shared_from_this());
            double marketStock = market.getInventory().getAmount(tmpTradable);
            // produce full only enough so that (market)stock+prod=clearing
            factory->produce(clearingInfo->second.amount - marketStock);
        }
    }
}

double Company::getMarketPrice(const std::shared_ptr<GenericCommodity>& good)
{
    auto& clearingInf = clearingByGood_.find(good);

    if (clearingInf != clearingByGood_.end())
    {
        return clearingInf->second.price;
    }
    else
    {
        throw;
    }
}

void Company::purchaseFromMarket(ConsumerMarket& market)
{
    for (auto& factory : factories_)
    {
        auto& commodity = factory->getCommodityInstance();

        auto& clearingInfo = clearingByGood_.find(commodity);

        if (clearingInfo != clearingByGood_.end())
        {
            double productAmount = clearingInfo->second.amount;
            for (const FactorsOfProduction::Input& fop : commodity->getCommodityDefinition()->getFactorsOfProduction().getInputs())
            {
                double currentAmountAtLocation = factory->inputInventory_->getAmount(fop.commodity_);

                double neededAmount = productAmount * fop.amount_ - currentAmountAtLocation;

                if (neededAmount > 0.0)
                {
                    std::map<TradableGood, double> purchases;
                    market.buyGeneric(fop.commodity_, neededAmount, shared_from_this(), purchases);

                    for (auto& purchase : purchases)
                    {
                        market.addTransportJob(purchase.first, purchase.second, market.getPickupInventory(), factory->inputInventory_);
                    }
                }
            }
        }
    }
}

void Company::newRound()
{
    clearingByGood_.clear();
}
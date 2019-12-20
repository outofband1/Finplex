#include "MarketSolver.h"
#include <iostream>
#include <random>
#include "CommodityInstance.h"
#include "CommodityDefinition.h"
#include "ConsumerMarket.h"
#include "TradableGood.h"

using namespace market_solver;

void Result::setAmountSold(double amountSold)
{
    amountSold_ = amountSold;
}

double Result::getAmountSold() const
{
    return amountSold_;
}

void Result::setPrice(double price)
{
    price_ = price;
}

double Result::getPrice() const
{
    return price_;
}

void Solver::clear()
{
    constraints_.clear();
}

void Solver::registerProduct(const TradableGood& tradableGood, const double & capacity, const double & minPrice)
{
    ProductConstraint newConstraint;

    newConstraint.maximumAmount_ = capacity;
    newConstraint.minimumPrice_ = minPrice;

    constraints_[tradableGood] = newConstraint;
}

void Solver::refreshMinimumPrices(std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance>& balance,
                                  std::map<TradableGood, PriceAndAmount>& pandas,
                                  const double& normalizationFactor)
{
    bool done = true;
    do // loop until stable
    {
        done = true;
        for (auto& com : balance)
        {
            double oldPrice = com.second.price;
            double minPrice = com.second.priceAdd;
            minPrice += com.first->getFactorsOfProduction().getLabour() * 10.0 * normalizationFactor; // TODO hourly wage

            for (auto& input : com.first->getFactorsOfProduction().getInputs())
            {
                auto& inputBalance = balance.find(input.commodity_);
                minPrice += inputBalance->second.price * input.amount_;
            }

            com.second.price = minPrice;

            if (oldPrice != minPrice)
            {
                done = false;
            }
        }
    }
    while (!done);

    for (auto& panda : pandas)
    {
        double minPrice = 0.0;
        auto& comDef = panda.first.getCommodity()->getCommodityDefinition();
        minPrice += comDef->getFactorsOfProduction().getLabour() * 10.0 * normalizationFactor; // TODO hourly wage
        for (auto& input : comDef->getFactorsOfProduction().getInputs())
        {
            auto& inputBalance = balance.find(input.commodity_);
            minPrice += inputBalance->second.price * input.amount_;
        }
        panda.second.minPrice_ = minPrice;
    }
}

void Solver::updateNeeded(std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance>& balance, std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance>::iterator& comBalance, double add)
{
    comBalance->second.needed += add;

    for (auto& input : comBalance->first->getFactorsOfProduction().getInputs())
    {
        auto& inputBalance = balance.find(input.commodity_);
        double needed = input.amount_ * add;
        updateNeeded(balance, inputBalance, needed);
    }
}

void Solver::refreshCommodityInput(std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance>& balance,
                                   const std::map<TradableGood, PriceAndAmount>& pandas)
{
    for (auto& com : balance)
    {
        com.second.needed = 0;
    }

    for (auto& panda : pandas)
    {
        auto& comDef = panda.first.getCommodity()->getCommodityDefinition();
        for (auto& input : comDef->getFactorsOfProduction().getInputs())
        {
            auto& inputBalance = balance.find(input.commodity_);

            double needed = input.amount_ * panda.second.amount_;
            updateNeeded(balance, inputBalance, needed);
        }
    }
}

void Solver::findPurchases(double scale, double budget, bool pricesLocked, std::map<TradableGood, Result>& results)
{
    /*
    normalization factor is used to bring budget and prices into a fixed range. This makes it much easier to select
    a proper line search length. Resolution does (theoretically?) suffer at higher budgets/prices.
    */
    double normalizationFactor = 100.0 / budget;

    budget *= normalizationFactor;

    if (constraints_.empty())
    {
        return;
    }

    std::map<std::shared_ptr<CommodityDefinition>, CommodityBalance> commoditiesBalance;
    std::map<TradableGood, PriceAndAmount > myPandas;

    for (auto& constraint : constraints_)
    {
        if (constraint.first.getCommodity()->getCommodityDefinition()->getUtilities().size() == 0)
        {
            auto& comDef = constraint.first.getCommodity()->getCommodityDefinition();

            auto& balance = commoditiesBalance.find(comDef);
            if (balance != commoditiesBalance.end())
            {
                commoditiesBalance[comDef].capacity += constraint.second.maximumAmount_;
            }
            else
            {
                commoditiesBalance[comDef].capacity = constraint.second.maximumAmount_;
                commoditiesBalance[comDef].needed = 0.0;
                commoditiesBalance[comDef].priceAdd = 0.0;
            }
        }

        else
        {
            if (constraint.second.maximumAmount_ > 10E-6)
            {
                PriceAndAmount workingPanda;
                workingPanda.amount_ = 0.0;
                workingPanda.mu_ = 0.0;
                workingPanda.minPrice_ = constraint.second.minimumPrice_ * normalizationFactor;
                workingPanda.capacity_ = constraint.second.maximumAmount_;
                workingPanda.price_ = constraint.second.minimumPrice_ * normalizationFactor;
                workingPanda.effectivePrice_ = (constraint.second.minimumPrice_ /*+ 10.0*/ /* TODO */) * normalizationFactor;

                workingPanda.active_ = true;

                myPandas[constraint.first] = workingPanda;
            }
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);

    bool done = false;
    while (!done)
    {
        if (!pricesLocked)
        {
            refreshMinimumPrices(commoditiesBalance, myPandas, normalizationFactor);
            for (auto& panda : myPandas)
            {
                panda.second.amount_ = 0.0;
                panda.second.price_ = panda.second.minPrice_;
            }
        }

        double spend = 0.0;

        refreshUtilities(scale, myPandas);

        int it = 0;
        double lastSpend = 0.0;
        while (spend <= budget)
        {
            it++;

            auto& topPanda = myPandas.begin();
            double topMuOverPrice = -1.0;

            for (auto& candidatePanda = myPandas.begin(); candidatePanda != myPandas.end(); candidatePanda++)
            {
                if (!candidatePanda->second.active_)
                {
                    continue;
                }
                // amount not important - just introduces a bit of variance when mu/p are equal
                // otherwise order of sold identical products always the same. This way distributes sales among sellers
                double shuffleEffect = distribution(g) * 10E-6;

                candidatePanda->second.effectivePrice_ = candidatePanda->second.price_;
                if (candidatePanda->first.getCommodity()->getCommodityDefinition()->isActivity())
                {
                    //candidatePanda->second.effectivePrice_ += 10.0 * normalizationFactor; // TODO hourly wage
                }

                double muOverPrice = candidatePanda->second.mu_ / candidatePanda->second.effectivePrice_ + shuffleEffect;
                if (muOverPrice > topMuOverPrice)
                {
                    topPanda = candidatePanda;
                    topMuOverPrice = muOverPrice;
                }
            }

            double priceLine = (budget - spend) * 0.001;
            double amountLine = (budget - spend) * 0.001;
            if (priceLine < 0.001)
            {
                priceLine = 0.001;
            }

            if (topPanda->second.amount_ + amountLine < topPanda->second.capacity_)
            {
                topPanda->second.amount_ += amountLine;
            }
            else
            {
                topPanda->second.amount_ = topPanda->second.capacity_;

                if (!pricesLocked)
                {
                    topPanda->second.price_ += priceLine;
                }
                else
                {
                    // nothing left to adjust, ignore in future iterations
                    topPanda->second.active_ = false;
                }
            }

            lastSpend = spend;
            spend = 0.0;
            for (auto& panda : myPandas)
            {
                spend += panda.second.price_ * panda.second.amount_;
            }

            refreshUtilities(scale, myPandas);

            if (spend <= lastSpend)
            {
                break;
            }
        }

        done = true;
        if (!pricesLocked)
        {
            refreshCommodityInput(commoditiesBalance, myPandas);

            for (auto& balance : commoditiesBalance)
            {
                if (balance.second.capacity < balance.second.needed)
                {
                    double line = (balance.second.needed - balance.second.capacity) * 0.1;

                    if (line > 0.5)
                    {
                        line = 0.5;
                    }

                    if (line < 0.01)
                    {
                        line = 0.01;
                    }
                    balance.second.priceAdd += line;
                    done = false;
                }
            }
        }
    }

    results.clear();

    // map consumer goods to results
    for (auto& panda : myPandas)
    {
        Result newResult;
        newResult.setAmountSold(panda.second.amount_);
        newResult.setPrice(panda.second.price_ / normalizationFactor);
        results[panda.first] = newResult;
    }

    // map commodities to results
    // if multiple same commodities split sales
    for (auto& balance : commoditiesBalance)
    {
        auto& comDef = balance.first;
        double needed = balance.second.needed;

        std::map <TradableGood, double> potentialItems;

        for (auto& constraint : constraints_)
        {
            if (constraint.first.getCommodity()->getCommodityDefinition() == comDef && constraint.second.maximumAmount_ > 10E-6)
            {
                potentialItems[constraint.first] = constraint.second.maximumAmount_;
            }
        }

        while (needed > 10E-6 && potentialItems.size() > 0)
        {
            int remainingPotentialSellers = static_cast<int>(potentialItems.size());
            double amountperSeller = needed / remainingPotentialSellers;

            for (std::map <TradableGood, double>::iterator item = potentialItems.begin(); item != potentialItems.end(); item++)
            {
                double maxAmount = item->second;
                double actualAmount = amountperSeller;

                bool itemDone = false;
                if (maxAmount <= amountperSeller)
                {
                    actualAmount = maxAmount;
                    itemDone = true;
                }

                needed -= actualAmount;

                Result newResult;
                newResult.setPrice(balance.second.price / normalizationFactor);
                newResult.setAmountSold(actualAmount);

                results[item->first] = newResult;

                if (itemDone)
                {
                    potentialItems.erase(item);
                    break;
                }
            }
        }
    }

}

void Solver::refreshUtilities(double scale, std::map<TradableGood, PriceAndAmount>& pandas)
{
    calculateUtilityAmounts(scale, pandas);

    for (auto& panda2 : pandas)
    {
        panda2.second.mu_ = getMarginalUtility(panda2.first.getCommodity());
    }
}

void Solver::calculateUtilityAmounts(double scale, std::map<TradableGood, PriceAndAmount>& pandas)
{
    utilityAmounts_.clear();
    for (auto& panda : pandas)
    {
        double amount = panda.second.amount_ / scale;
        for (auto& util : panda.first.getCommodity()->getCommodityDefinition()->getUtilities())
        {
            double weight = util.second;
            auto& utilityAmount = utilityAmounts_.find(util.first);
            if (utilityAmount != utilityAmounts_.end())
            {
                utilityAmount->second += weight * amount;
            }
            else
            {
                utilityAmounts_[util.first] = weight * amount;
            }
        }
    }
}

double Solver::getMarginalUtility(const std::shared_ptr<GenericCommodity>& product) const
{
    double mu = 0.0;
    for (const auto& util : product->getCommodityDefinition()->getUtilities())
    {
        auto& it = utilityAmounts_.find(util.first);
        double utilityAmount = it->second;
        mu += util.second * util.first->getMarginalUtility(utilityAmount);
    }

    return mu;
}

void market_solver::printResults(const std::map<TradableGood, Result> & results)
{
    double spend = 0.0;
    for (auto& result : results)
    {
        std::cout << "Product: " << result.first.getCommodity()->getName() << " (" << result.first.getCommodity()->getCommodityDefinition()->getName() << ")" << std::endl;
        double amount = result.second.getAmountSold();
        double price = result.second.getPrice();

        std::cout << "Amount: " << amount << " at price: " << price << std::endl;

        spend += amount * price;
    }

    std::cout << "Total: " << spend << std::endl;
}
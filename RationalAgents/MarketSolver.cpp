#include "MarketSolver.h"
#include <iostream>
#include <random>

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

void Solver::registerProduct(const std::shared_ptr<CommodityInstance> & product, const double & capacity, const double & minPrice)
{
    ProductConstraint newConstraint;
    newConstraint.maximumAmount_ = capacity;
    newConstraint.minimumPrice_ = minPrice;

    constraints_[product] = newConstraint;
}

void Solver::findPurchases(double scale, double budget, bool pricesLocked, std::map<std::shared_ptr<CommodityInstance>, Result>& results)
{
    results.clear();

    std::map<std::shared_ptr<CommodityInstance>, PriceAndAmount> myPandas;

    for (auto& constraint : constraints_)
    {
        if (constraint.second.maximumAmount_ > 10E-6)
        {
            PriceAndAmount workingPanda;
            workingPanda.amount_ = 0.0;
            workingPanda.mu_ = 0.0;
            workingPanda.minPrice_ = constraint.second.minimumPrice_;
            workingPanda.capacity_ = constraint.second.maximumAmount_;
            workingPanda.price_ = constraint.second.minimumPrice_;

            workingPanda.active_ = true;

            myPandas[constraint.first] = workingPanda;
        }
    }

    double spend = 0.0;

    refreshUtilities(scale, myPandas);

    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);

    int it = 0;
    double lastSpend = 0.0;
    while (spend <= budget)
    {
        it++;

        auto& topPanda = myPandas.begin();
        double topMuOverPrice = -1.0;

        int candidateIndex = 1;
        for (auto& candidatePanda = myPandas.begin(); candidatePanda != myPandas.end(); candidatePanda++)
        {
            if (!candidatePanda->second.active_)
            {
                continue;
            }
            // amount not important - just introduces a bit of variance when mu/p are equal
            double shuffleEffect = distribution(g) * 10E-6;

            double muOverPrice = candidatePanda->second.mu_ / candidatePanda->second.price_ + shuffleEffect;
            if (muOverPrice > topMuOverPrice)
            {
                topPanda = candidatePanda;
                topMuOverPrice = muOverPrice;
            }
        }

        if (topMuOverPrice < 0.0)
        {
            break;
        }

        //double line = (budget - spend) / budget;
        double line = (budget - spend) * 0.01;
        if (line < 0.001)
        {
            line = 0.001;
        }

        if (topPanda->second.amount_ + 0.1 * line < topPanda->second.capacity_)
        {
            topPanda->second.amount_ += 0.1 * line;
        }
        else
        {
            topPanda->second.amount_ = topPanda->second.capacity_;

            if (!pricesLocked)
            {
                topPanda->second.price_ += 0.1 * line;
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

    for (auto& panda : myPandas)
    {
        Result newResult;
        newResult.setAmountSold(panda.second.amount_);
        newResult.setPrice(panda.second.price_);
        results[panda.first] = newResult;
    }

    std::cout << "ITERATIONS: " << it << std::endl;

    /*double totalSpend = 0.0;

    for (auto& panda : myPandas)
    {
    std::cout << panda.first->getName() << ":" << std::endl;
    std::cout << panda.second.amount_ << " at price: " << panda.second.price_ << std::endl;
    std::cout << panda.second.mu_ / panda.second.price_ << std::endl << std::endl;

    totalSpend += panda.second.amount_ * panda.second.price_;
    }

    std::cout << "Total spend: " << totalSpend << std::endl << std::endl;*/
}

void Solver::refreshUtilities(double scale, std::map<std::shared_ptr<CommodityInstance>, PriceAndAmount>& pandas)
{
    calculateUtilityAmounts(scale, pandas);

    for (auto& panda2 : pandas)
    {
        panda2.second.mu_ = getMarginalUtility(panda2.first);
    }
}

void Solver::calculateUtilityAmounts(double scale, std::map<std::shared_ptr<CommodityInstance>, PriceAndAmount>& pandas)
{
    utilityAmounts_.clear();
    for (auto& panda : pandas)
    {
        double amount = panda.second.amount_ / scale;
        for (auto& util : panda.first->getCommodity()->getUtilities())
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

double Solver::getMarginalUtility(const std::shared_ptr<CommodityInstance>& product) const
{
    double mu = 0.0;
    for (const auto& util : product->getCommodity()->getUtilities())
    {
        auto& it = utilityAmounts_.find(util.first);
        double utilityAmount = it->second;
        mu += util.second * util.first->getMarginalUtility(utilityAmount);
    }

    return mu;
}

void market_solver::printResults(const std::map<std::shared_ptr<CommodityInstance>, Result>& results)
{
    double spend = 0.0;
    for (auto& result : results)
    {
        std::cout << "Product: " << result.first->getName() << " (" << result.first->getCommodity()->getName() << ")" << std::endl;
        double amount = result.second.getAmountSold();
        double price = result.second.getPrice();

        std::cout << "Amount: " << amount << " at price: " << price << std::endl;

        spend += amount * price;
    }

    std::cout << "Total: " << spend << std::endl;
}
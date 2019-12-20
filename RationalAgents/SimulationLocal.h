#pragma once
#include <memory>
#include "Factory.h"
#include "MarketSolver.h"
#include <iostream>
#include "ConsumerMarket.h"
#include "World.h"
#include "CommodityDefinition.h"

/*



Questions:
Increasing hired labour will potentially lower profit. Should you still do it?
Not doing it means cut-throat-monopoly, in a competitive market always use max capacity if possible
Assume competitive market for now

Labour, wages and production are never exactly in balance. Worst when number of agents/producers is low.
Solutions:
introduce taxes and welfare to "soften" effects out
introduce stealing
introduce charity between agents


Ideas:
Bigger factories produce research -> leads to possible patents and related mechanics
*/




class SimulationLocal
{
public:

    void acceptGoodForMarketAnalysis(const TradableGood& tradableGood, const double amount)
    {
        if (amount > 10E-6)
        {
            solver.registerProduct(tradableGood, amount, 0.0);
        }
    }

    void acceptAgentBudgetForMarketAnalysis(const double budget)
    {
        allAgentsBudget += budget;
    }

    ConsumerMarket& getMarket()
    {
        return market_;
    }

    void main(World& world)
    {

        /*
        TODO

        commodities with time cost
            compare to ordinary goods but respect free time constraint
            how to handle free activities with no mu/price (maybe mu/time ? )
                is the price equal to wage even though jobs are limited?

        employment and wages !!

        better transportation
            think about pricing

        savings

        is it possible to find labor-price / wage while solving????
            no because all other prices are determined in relation to wages

        if in transition, produce more than is currently needed, expecting to clear in future transactions
            complicated i think
        */

        // initialize new round
        allAgentsBudget = 0.0;
        market_.newRound();
        for (auto& company : world.getCompanies())
        {
            company->newRound();
        }

        for (auto& agent : world.getAgents())
        {
            agent->newRound();
        }
        // done initializing new round

        for (auto& agent : world.getAgents())
        {
            agent->money_ += 0.9 * 80.0; // TODO handle employment and stuff
            agent->submitBudgetForMarketAnalysis(*this);
        }

        int populationSize = static_cast<int>(world.getAgents().size());

        for (auto& company : world.getCompanies())
        {
            company->submitGoodsForMarketAnalysis(*this);
        }

        // determine market clearing prices
        std::map<TradableGood, market_solver::Result> results;
        solver.findPurchases(populationSize, allAgentsBudget, false, results);

        for (auto& result : results)
        {
            double amount = result.second.getAmountSold();
            double price = result.second.getPrice();

            // company
            auto& owner = result.first.getOwner();

            owner->receiveClearingInformation(result.first.getCommodity(), amount, price);
        }

        for (auto& company : world.getCompanies())
        {
            company->produce(market_);
        }

        for (auto& factory : world.getFactories())
        {
            factory->productInventory_->transferAllTo(market_.getInventory());
        }

        market_.updatePrices();

        for (auto& company : world.getCompanies())
        {
            company->purchaseFromMarket(market_);
        }

        // TODO hmhmhmhmh
        for (auto& transportJob : market_.getTransportJobs())
        {
            std::shared_ptr<Inventory> source = transportJob.first->getSource();
            std::shared_ptr<Inventory> dest = transportJob.first->getDestination();

            market_.acceptTransportJob(transportJob.first);

            source->transferTo(transportJob.first->getTradableGood(), *dest, transportJob.first->getTradableGood().getOwner(), transportJob.first->getAmount());
        }

        for (auto& agent : world.getAgents())
        {
            auto& marketItems = market_.getItems();

            solver.clear();
            for (auto& item : marketItems)
            {
                // only end consumer goods need to be included here
                if (item.tradableGood_.getCommodity()->getCommodityDefinition()->getUtilities().size() > 0 && item.amount_ > 10E-6)
                {
                    solver.registerProduct(item.tradableGood_, item.amount_, item.price_);
                }
            }

            std::map<TradableGood, market_solver::Result> results;
            solver.findPurchases(1, agent->money_, true, results);

            for (auto& result : results)
            {
                double amount = result.second.getAmountSold();
                double price = result.second.getPrice();

                if (amount > 10E-6)
                {
                    market_.makeTransaction(result.first, agent, amount, price);
                }
            }
        }

        auto& marketItems = market_.getItems();

        std::cout << std::endl;
        std::cout << "Market Inventory:" << std::endl;
        for (auto& item : marketItems)
        {
            std::cout << item.amount_ << " " << item.tradableGood_.getCommodity()->getName() << " (" << item.price_ << ")" <<  std::endl;
        }

        for (auto& factory : world.getFactories())
        {
            std::cout << std::endl;

            std::cout << "Producing:" << factory->getCommodityInstance()->getName() << std::endl;

            factory->inputInventory_->print();

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }


private:
    ConsumerMarket market_;
    market_solver::Solver solver;

    double allAgentsBudget;
};
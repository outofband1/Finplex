#include <tchar.h>
#include <iostream>
#include <memory>
#include <string>

#include "DefinitionStorage.h"
#include "utility.h"
#include "Commodity.h"
#include "CommodityInstance.h"
#include "CommodityProducer.h"
#include "MarketSolver.h"
#include <vector>


/*

1. have producers interact with the optimizer
    a. just fixed production and labour
    b. use solver to determine if labour should be increased
        * step 2 -> compete for labour by adjusting wage if needed

Questions:
    Increasing hired labour will potentially lower profit. Should you still do it?
        Not doing it means cut-throat-monopoly, in a competitive market always use max capacity if possible
            Assume competitive market for now

Ideas:
    Bigger factories produce research -> leads to possible patents and related mechanics
*/

class SimulationLocal
{
public:
    void registerProducer(const std::shared_ptr<CommodityProducer>& producer)
    {
        const std::shared_ptr<CommodityInstance>& commodity = producer->getCommodityInstance();
        commodityProducers_[commodity] = producer;
    }

    void main()
    {
        market_solver::Solver solver;
        for (auto& commodity : commodityProducers_)
        {
            const double& minPrice = commodity.second->getMinimumSellingPrice();
            const double& capacity = commodity.second->getCapacity();

            solver.registerProduct(commodity.first, capacity, minPrice);
        }

        double totalBudget = population * workingHours * hourlyWage;

        std::map<std::shared_ptr<CommodityInstance>, market_solver::Result> results;
        solver.findPurchases(population, totalBudget, false, results);

        for (auto& result : results)
        {
            auto& producer = commodityProducers_[result.first];

            double amount = result.second.getAmountSold();
            double price = result.second.getPrice();

            producer->setCurrentMaxSalesAndPrice(amount, price);
        }

        market_solver::printResults(results);
    }

private:
    double population = 3;
    double hourlyWage = 10.0;
    double workingHours = 8.0;

    std::map<std::shared_ptr<CommodityInstance>, std::shared_ptr<CommodityProducer>> commodityProducers_;
};

int _tmain(int argc, _TCHAR* argv[])
{
    DefinitionStorage definitions;
    definitions.readDefinitions();

    std::shared_ptr<CommodityInstance> meal1 = std::make_shared<CommodityInstance>("Meal 1", definitions.getCommodity("meal"));
    //std::shared_ptr<CommodityInstance> meal2 = std::make_shared<CommodityInstance>("Meal 2", definitions.getCommodity("meal"));
    std::shared_ptr<CommodityInstance> beer1 = std::make_shared<CommodityInstance>("Beer 1", definitions.getCommodity("beer"));
    //std::shared_ptr<CommodityInstance> beer2 = std::make_shared<CommodityInstance>("Beer 2", definitions.getCommodity("beer"));
    std::shared_ptr<CommodityInstance> relax1 = std::make_shared<CommodityInstance>("Luxuries", definitions.getCommodity("luxuries"));

    std::shared_ptr<CommodityProducer> p1 = std::make_shared<CommodityProducer>("P1", definitions.getCommodity("meal"));
    std::shared_ptr<CommodityProducer> p2 = std::make_shared<CommodityProducer>("P2", definitions.getCommodity("beer"));
    std::shared_ptr<CommodityProducer> p3 = std::make_shared<CommodityProducer>("P3", definitions.getCommodity("luxuries"));

    SimulationLocal sim;
    sim.registerProducer(p1);
    sim.registerProducer(p2);
    sim.registerProducer(p3);

    sim.main();

    market_solver::Solver opt;

    std::map<std::shared_ptr<CommodityInstance>, market_solver::Result> results;

    double hourlyWage = 10.0;
    double mealProductionTime = definitions.getCommodity("meal")->getFactorsOfProduction().getLabour();
    double beerProductionTime = definitions.getCommodity("beer")->getFactorsOfProduction().getLabour();
    double relaxProductionTime = definitions.getCommodity("luxuries")->getFactorsOfProduction().getLabour();

    double workingHours = 8.0;

    double population = 3;

    double mealMinPrice = hourlyWage * mealProductionTime;
    double beerMinPrice = hourlyWage * beerProductionTime;
    double relaxMinPrice = hourlyWage * relaxProductionTime;

    double totalBudget = population * workingHours * hourlyWage;

    //for (int j = 0; j < 1; j++)
    {
        opt.registerProduct(meal1, p1->getCapacity(), p1->getMinimumSellingPrice());
        //opt.registerProduct(meal2, p1.getCapacity(), p1.getMinimumSellingPrice());
        opt.registerProduct(beer1, p2->getCapacity(), p2->getMinimumSellingPrice());
        //opt.registerProduct(beer2, p2.getCapacity(), p2.getMinimumSellingPrice());
        opt.registerProduct(relax1, p3->getCapacity(), p3->getMinimumSellingPrice());

        opt.findPurchases(population, totalBudget, false, results);

        market_solver::printResults(results);

        double meal1Available = results[meal1].getAmountSold();
        //double meal2Available = results[meal2].getAmountSold();
        double beer1Available = results[beer1].getAmountSold();
        //double beer2Available = results[beer2].getAmountSold();
        double relax1Available = results[relax1].getAmountSold();

        for (int i = 0; i < population; i++)
        {
            opt.registerProduct(meal1, meal1Available, results[meal1].getPrice());
            //opt.registerProduct(meal2, meal2Available, results[meal2].getPrice());
            opt.registerProduct(beer1, beer1Available, results[beer1].getPrice());
            //opt.registerProduct(beer2, beer2Available, results[beer2].getPrice());
            opt.registerProduct(relax1, relax1Available, results[relax1].getPrice());

            opt.findPurchases(1, workingHours * hourlyWage, true, results);

            market_solver::printResults(results);

            double meal1Sold = results[meal1].getAmountSold();
            //double meal2Sold = results[meal2].getAmountSold();
            double beer1Sold = results[beer1].getAmountSold();
            //double beer2Sold = results[beer2].getAmountSold();
            double relax1Sold = results[relax1].getAmountSold();

            meal1Available -= meal1Sold;
            //meal2Available -= meal2Sold;
            beer1Available -= beer1Sold;
            //beer2Available -= beer2Sold;
            relax1Available -= relax1Sold;
        }
    }





    system("pause");

    return 0;
}





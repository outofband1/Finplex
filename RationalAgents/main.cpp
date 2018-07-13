#include "Simplex.h"
#include <tchar.h>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "Utility.h"
#include "Commodity.h"
#include "Activity.h"
#include "PurchaseSolver.h"
#include "DefinitionStorage.h"
#include "Inventory.h"
#include "Producer.h"
#include <algorithm>
#include "Entity.h"
#include "Market.h"
#include "TradableGood.h"

int _tmain(int argc, _TCHAR* argv[])
{
    DefinitionStorage defStore;
    defStore.readDefinitions();

    PurchaseSolver solver;

    for (auto& utility : defStore.getUtilityDefinitions())
    {
        solver.registerUtility(utility.second);
    }

    std::shared_ptr<Commodity> meal1 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal1->setName("Meal 1");
    std::shared_ptr<Producer> p1 = std::make_shared<Producer>("Meal Producer 1", meal1);
    p1->addCurrency(1000.0);

    std::shared_ptr<Commodity> meal2 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal2->setName("Meal 2");
    std::shared_ptr<Producer> p2 = std::make_shared<Producer>("Meal Producer 2", meal2);
    p2->addCurrency(1000.0);

    std::shared_ptr<Commodity> meal3 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal3->setName("Meal 3");
    std::shared_ptr<Producer> p3 = std::make_shared<Producer>("Meal Producer 3", meal3);
    p3->addCurrency(1000.0);

    std::shared_ptr<Commodity> beer1 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("beer")->second));
    beer1->setName("Beer 1");
    std::shared_ptr<Producer> p4 = std::make_shared<Producer>("Beer Producer 1", beer1);
    p4->addCurrency(1000.0);

    std::shared_ptr<Commodity> beer2 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("beer")->second));
    beer2->setName("Beer 2");
    std::shared_ptr<Producer> p5 = std::make_shared<Producer>("Beer Producer 2", beer2);
    p5->addCurrency(1000.0);

    Inventory inv;

    std::map<std::shared_ptr<Commodity>, double> commodityPurchases;
    std::map<std::shared_ptr<Activity>, double> activityPurchases;

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("savings")->second, 1.0f, -1.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("leisure")->second, 0.5f, 3.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("sleep")->second, 0.0f, -1.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("relaxation")->second, 0.0f, -1.0);

    std::vector<std::shared_ptr<Producer>> prods;
    prods.push_back(p1);
    prods.push_back(p2);
    //prods.push_back(p3);
    prods.push_back(p4);
    prods.push_back(p5);

    Market market;

    std::shared_ptr<Entity> e1 = std::make_shared<Entity>("Bob");
    e1->addCurrency(0);

    double wage = 0.2;
    int count = 0;
    while (true)
    {
        count++;
        std::cout << "COUNT: -------- " << count << " --------" << std::endl;
        for (int i = 0; i < 10; i++)
        {
            std::random_shuffle(prods.begin(), prods.end());
            for (auto& p : prods)
            {
                p->optimizeProductionAndPrice(solver, market, e1->getCurrency());
            }

            solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor 1")->second, -wage, p1->plannedProductionAmount_ * 3);
            solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor 2")->second, -wage, p2->plannedProductionAmount_ * 3);
            //solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor 3")->second, -wage, p3->plannedProductionAmount_);
            solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor 4")->second, -wage, p4->plannedProductionAmount_ * 3);
            solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor 5")->second, -wage, p5->plannedProductionAmount_ * 3);
        }

        for (auto& p : prods)
        {
            p->goToMarket(market);
            std::cout << p->getName() << ": " << p->getCurrency() << std::endl;
        }

        market.print();

        std::map < std::shared_ptr<Commodity>, std::shared_ptr<TradableGood>> pmap;
        for (auto& item : market.inventory_)
        {
            std::shared_ptr<TradableGood> good = item.first;
            const std::shared_ptr<Commodity>& commodity = good->getCommodity();
            double price = market.getSellingPrice(good);
            double amount = item.second;

            pmap[commodity] = good;
            solver.setPriceAndAmount(commodity, price, amount);
        }

        solver.OptimizeTimeAndPurchases(24, e1->getCurrency(), inv, commodityPurchases, activityPurchases);


        for (auto& purchase : activityPurchases)
        {
            std::cout << purchase.first->getName() << " : " << purchase.second << std::endl;
            if (purchase.first->getName() == "Unskilled labor 1")
            {
                e1->addCurrency(purchase.second * 3 * wage);
                p1->removeCurrency(purchase.second * 3 * wage);
            }
            else if (purchase.first->getName() == "Unskilled labor 2")
            {
                e1->addCurrency(purchase.second * 3 * wage);
                p2->removeCurrency(purchase.second * 3 * wage);
            }
            else if (purchase.first->getName() == "Unskilled labor 3")
            {
                e1->addCurrency(purchase.second * 3 * wage);
                p3->removeCurrency(purchase.second * 3 * wage);
            }
            else if (purchase.first->getName() == "Unskilled labor 4")
            {
                e1->addCurrency(purchase.second * 3 * wage);
                p4->removeCurrency(purchase.second * 3 * wage);
            }
            else if (purchase.first->getName() == "Unskilled labor 5")
            {
                e1->addCurrency(purchase.second * 3 * wage);
                p5->removeCurrency(purchase.second * 3 * wage);
            }
        }

        for (auto& purchase : commodityPurchases)
        {
            std::cout << purchase.first->getName() << " : " << purchase.second << std::endl;
            if (pmap.find(purchase.first) != pmap.end())
            {
                market.sell(pmap[purchase.first], e1, purchase.second);
            }
        }

        std::cout << e1->getCurrency() << std::endl;
        system("pause");
        int abe = 6;
    }

    std::cout << std::endl;
    system("pause");
    return 0;
}





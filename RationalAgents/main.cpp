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

int _tmain(int argc, _TCHAR* argv[])
{
    DefinitionStorage defStore;
    defStore.readDefinitions();

    PurchaseSolver solver;

    for (auto& utility : defStore.getUtilityDefinitions())
    {
        solver.registerUtility(utility.second);
    }

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("savings")->second, 1.0f, -1.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("leisure")->second, 0.5f, 3.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor")->second, -0.8f, 5.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("skilled labor")->second, -0.8f, 3.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("relaxation")->second, 0.0f, -1.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("sleep")->second, 0.0f, -1.0);

    std::shared_ptr<Commodity> meal1 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal1->setName("Meal 1");
    std::shared_ptr<Producer> p1 = std::make_shared<Producer>(meal1);

    std::shared_ptr<Commodity> meal2 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal2->setName("Meal 2");
    std::shared_ptr<Producer> p2 = std::make_shared<Producer>(meal2);

    std::shared_ptr<Commodity> meal3 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal3->setName("Meal 3");
    std::shared_ptr<Producer> p3 = std::make_shared<Producer>(meal3);

    std::shared_ptr<Commodity> beer1 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("beer")->second));
    beer1->setName("Beer 1");
    std::shared_ptr<Producer> p4 = std::make_shared<Producer>(beer1);

    std::shared_ptr<Commodity> beer2 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("beer")->second));
    beer2->setName("Beer 2");
    std::shared_ptr<Producer> p5 = std::make_shared<Producer>(beer2);

    Inventory inv;

    std::map<std::shared_ptr<Commodity>, double> commodityPurchases;
    std::map<std::shared_ptr<Activity>, double> activityPurchases;

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("savings")->second, 1.0f, -1.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("leisure")->second, 0.5f, 3.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor")->second, -0.8, 7.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("skilled labor")->second, -0.8f, 5.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("relaxation")->second, 0.0f, -1.0);

    std::vector<std::shared_ptr<Producer>> prods;
    prods.push_back(p1);
    prods.push_back(p2);
    prods.push_back(p3);
    prods.push_back(p4);

    int count = 0;
    while (true)
    {
        count++;
        std::cout << "COUNT: -------- " << count << " --------" << std::endl;
        std::random_shuffle(prods.begin(), prods.end());
        for (auto& p : prods)
        {
            p->planProduction(solver, 0, 0);
        }

        solver.OptimizeTimeAndPurchases(24, 0.0, inv, commodityPurchases, activityPurchases);

        for (auto& purchase : activityPurchases)
        {
            std::cout << purchase.first->getName() << " : " << purchase.second << std::endl;
        }

        for (auto& purchase : commodityPurchases)
        {
            std::cout << purchase.first->getName() << " : " << purchase.second << std::endl;
        }

        //system("pause");
        int abe = 6;
    }

    std::cout << std::endl;
    system("pause");
    return 0;
}





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

int _tmain(int argc, _TCHAR* argv[])
{
    DefinitionStorage defStore;
    defStore.readDefinitions();

    PurchaseSolver solver;

    for (auto& utility : defStore.getUtilityDefinitions())
    {
        solver.registerUtility(utility.second);
    }

    //solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("meal")->second, 1.0f, 1.0f);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("beer")->second, 0.4f, 40.0f);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("savings")->second, 1.0f, -1.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("leisure")->second, 0.5f, 3.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor")->second, -0.8f, 5.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("skilled labor")->second, -0.8f, 3.0);

    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("relaxation")->second, 0.0f, -1.0);

    std::shared_ptr<Commodity> meal1 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal1->setName("Meal 1");
    Producer p1(meal1);

    std::shared_ptr<Commodity> meal2 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal2->setName("Meal 2");
    Producer p2(meal2);

    std::shared_ptr<Commodity> meal3 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal3->setName("Meal 3");
    Producer p3(meal3);

    Inventory inv;

    std::map<std::shared_ptr<Commodity>, double> commodityPurchases;
    std::map<std::shared_ptr<Activity>, double> activityPurchases;

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("beer")->second, 0.4f, 40.0f);
    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("savings")->second, 1.0f, -1.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("leisure")->second, 0.5f, 3.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor")->second, -0.8, 5.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("skilled labor")->second, -0.8f, 3.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("relaxation")->second, 0.0f, -1.0);

    while (true)
    {
        p1.planProduction(solver, 0.0, 0.0);
        p2.planProduction(solver, 0.0, 0.0);
        p3.planProduction(solver, 0.0, 0.0);

        solver.OptimizeTimeAndPurchases(24, 0.0, inv, commodityPurchases, activityPurchases);

        for (auto& purchase : commodityPurchases)
        {
            std::cout << purchase.first->getName() << " : " << purchase.second << std::endl;
        }

        system("pause");
        int abe = 6;
    }

    std::cout << std::endl;
    system("pause");
    return 0;
}





#include "Simplex.h"
#include <tchar.h>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "Utility.h"
#include "Commodity.h"
#include "PurchaseSolver.h"
#include "DefinitionStorage.h"

int _tmain(int argc, _TCHAR* argv[])
{
    DefinitionStorage defStore;
    defStore.readDefinitions();

    float money = 0;

    PurchaseSolver solver;


    for (auto& commodity : defStore.getCommodityDefinitions())
    {
        solver.registerCommodity(commodity.second);
    }

    for (auto& utility : defStore.getUtilityDefinitions())
    {
        solver.registerUtility(utility.second);
    }

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("meal")->second, 1.1f, 3.0f);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("beer")->second, 1.2f, 4.0f);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("savings")->second, 1.0f, -1.0);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("work")->second, -1.0f, 12.0f);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("leisure")->second, 1.0f, -1.0f);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("sleep")->second, 1.0f, -1.0f);

    std::map<std::shared_ptr<Commodity>, float> purchases;

    for (int i = 0; i < 1; i++)
    {
        solver.OptimizeTimeAndPurchases(money, purchases);
    }

    for (auto& purchase : purchases)
    {
        std::cout << purchase.first->getName() << ": " << purchase.second << std::endl;
    }

    std::cout << std::endl;
    system("pause");
    return 0;
}





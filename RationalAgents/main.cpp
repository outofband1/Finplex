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

int _tmain(int argc, _TCHAR* argv[])
{
    DefinitionStorage defStore;
    defStore.readDefinitions();

    PurchaseSolver solver;


    for (auto& commodity : defStore.getCommodityDefinitions())
    {
		solver.registerUtilitySource(commodity.second);
    }

	for (auto& activity : defStore.getActivityDefinitions())
	{
		solver.registerUtilitySource(activity.second);
	}

    for (auto& utility : defStore.getUtilityDefinitions())
    {
        solver.registerUtility(utility.second);
    }

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("meal")->second, 1.0f, 1.0f);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("beer")->second, 0.4f, 40.0f);

    solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("savings")->second, 1.0f, -1.0);

	solver.setPriceAndAmount(defStore.getActivityDefinitions().find("leisure")->second, 0.5f, 3.0);

	solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor")->second, -0.8f, 5.0);

	solver.setPriceAndAmount(defStore.getActivityDefinitions().find("skilled labor")->second, -1.1f, 2.0);

	solver.setPriceAndAmount(defStore.getActivityDefinitions().find("relaxation")->second, 0.0f, -1.0);

    std::map<std::shared_ptr<Commodity>, float> commodityPurchases;
	std::map<std::shared_ptr<Activity>, float> activityPurchases;

	Inventory inventory;
	float money = 0;

	std::cout.precision(10);

	inventory.add(defStore.getCommodityDefinitions().find("meal")->second, 0);

	float produce = 2.5;
	float produceMax = 10;
	float price = 1.33;
	float priceMin = 0.8;


    for (int i = 0; i < 1; i++)
    {
		solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("meal")->second, price, produce);

		std::cout << "Price: " << price << ", Produced: " << produce << std::endl;

		solver.OptimizeTimeAndPurchases(24, money, inventory, commodityPurchases, activityPurchases);

		for (auto& commodity : commodityPurchases)
		{
			std::cout << commodity.first->getName() << ": " << commodity.second << std::endl;

		}

		for (auto& activity : activityPurchases)
		{
			std::cout << activity.first->getName() << ": " << activity.second << std::endl;
		}

		auto& mealsBought = commodityPurchases.find(defStore.getCommodityDefinitions().find("meal")->second);
		
		std::cout << "PROFIT: " << mealsBought->second*price << std::endl;

		system("pause");
    }

	price += 0.01;
	for (int i = 0; i < 1; i++)
	{
		solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("meal")->second, price, produce);

		std::cout << "Price: " << price << ", Produced: " << produce << std::endl;

		solver.OptimizeTimeAndPurchases(24, money, inventory, commodityPurchases, activityPurchases);

		for (auto& commodity : commodityPurchases)
		{
			std::cout << commodity.first->getName() << ": " << commodity.second << std::endl;

		}

		for (auto& activity : activityPurchases)
		{
			std::cout << activity.first->getName() << ": " << activity.second << std::endl;
		}

		auto& mealsBought = commodityPurchases.find(defStore.getCommodityDefinitions().find("meal")->second);

		std::cout << "PROFIT: " << mealsBought->second*price << std::endl;

		system("pause");
	}

	price -= 0.01;
	produce += 0.01;
	for (int i = 0; i < 1; i++)
	{
		solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("meal")->second, price, produce);

		std::cout << "Price: " << price << ", Produced: " << produce << std::endl;

		solver.OptimizeTimeAndPurchases(24, money, inventory, commodityPurchases, activityPurchases);

		for (auto& commodity : commodityPurchases)
		{
			std::cout << commodity.first->getName() << ": " << commodity.second << std::endl;

		}

		for (auto& activity : activityPurchases)
		{
			std::cout << activity.first->getName() << ": " << activity.second << std::endl;
		}

		auto& mealsBought = commodityPurchases.find(defStore.getCommodityDefinitions().find("meal")->second);

		std::cout << "PROFIT: " << mealsBought->second*price << std::endl;

		system("pause");
	}



    std::cout << std::endl;
    system("pause");
    return 0;
}





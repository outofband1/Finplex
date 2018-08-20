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

#include "lp_lib.h"

struct pAndA
{
	double price;
	double amount;
	bool priceLocked;
};

int _tmain(int argc, _TCHAR* argv[])
{
    PurchaseSolver pSolver;

    DefinitionStorage defStore;
    defStore.readDefinitions();

    PurchaseSolver solver;

    for (auto& utility : defStore.getUtilityDefinitions())
    {
        solver.registerUtility(utility.second);
    }

    std::shared_ptr<Commodity> meal1 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal1->setName("m1");
    std::shared_ptr<Producer> p1 = std::make_shared<Producer>("Meal Producer 1", meal1);
    p1->addCurrency(1000.0);

    std::shared_ptr<Commodity> meal2 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal2->setName("m2");
    std::shared_ptr<Producer> p2 = std::make_shared<Producer>("Meal Producer 2", meal2);
    p2->addCurrency(1000.0);

    std::shared_ptr<Commodity> meal3 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("meal")->second));
    meal3->setName("m3");
    std::shared_ptr<Producer> p3 = std::make_shared<Producer>("Meal Producer 3", meal3);
    p3->addCurrency(1000.0);

    std::shared_ptr<Commodity> beer1 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("beer")->second));
    beer1->setName("b1");
    std::shared_ptr<Producer> p4 = std::make_shared<Producer>("Beer Producer 1", beer1);
    p4->addCurrency(1000.0);

    std::shared_ptr<Commodity> beer2 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("beer")->second));
    beer2->setName("b2");
    std::shared_ptr<Producer> p5 = std::make_shared<Producer>("Beer Producer 2", beer2);
    p5->addCurrency(1000.0);

	std::shared_ptr<Commodity> beer3 = std::make_shared<Commodity>(*(defStore.getCommodityDefinitions().find("beer")->second));
	beer3->setName("b3");
	
	

    Inventory inv;

    std::map<std::shared_ptr<Commodity>, double> commodityPurchases;
    std::map<std::shared_ptr<Activity>, double> activityPurchases;

    //solver.setPriceAndAmount(defStore.getCommodityDefinitions().find("savings")->second, 1.0f, -1.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("leisure")->second, 0.2f, 3.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("sleep")->second, 0.0f, -1.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("relaxation")->second, 0.0f, -1.0);
    solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor 1")->second, -1.5, 10);

	std::map<std::shared_ptr<Commodity>, pAndA> pandas;
	const double capacity = 2.0;
	pandas[meal1] = { 1.0, capacity, false };
	pandas[meal2] = { 1.0, capacity, false };
	//pandas[meal3] = { 1.0, capacity, false };
	pandas[beer1] = { 1.0, capacity/2.0, false };
	pandas[beer2] = { 1.0, capacity/2.0, false };
	//pandas[beer3] = { 1.0, capacity, false };

	/*
	TODO !!!


	each iteration
		if sold < produced (by a margin?)
			set amount available of sold good in solver to new amount
			set all other products to capacity

			optimize with inf money and respect labor constraints
	*/


	for (auto& panda : pandas)
	{
		// set maximum amount, and arbitrary price (price constraint deactivated below)
		solver.setPriceAndAmount(panda.first, panda.second.price, panda.second.amount);
	}

	solver.OptimizeTimeAndPurchases2(24.0, 0.0, true, inv, commodityPurchases, activityPurchases);

	double labor = 0;
	for (auto& panda : pandas)
	{
		//panda.second.amount = ;
		labor += commodityPurchases.find(panda.first)->second;

	}

	solver.setPriceAndAmount(defStore.getActivityDefinitions().find("unskilled labor 1")->second, -2.0, labor);

	std::map<std::shared_ptr<Commodity>, double> orgProfit;

	orgProfit[meal1] = 0.0;
	orgProfit[meal2] = 0.0;
	orgProfit[meal3] = 0.0;
	orgProfit[beer1] = 0.0;
	orgProfit[beer2] = 0.0;
	orgProfit[beer3] = 0.0;

	std::map<std::shared_ptr<Commodity>, double> newProfit;

	newProfit[meal1] = 0.0;
	newProfit[meal2] = 0.0;
	newProfit[meal3] = 0.0;
	newProfit[beer1] = 0.0;
	newProfit[beer2] = 0.0;
	newProfit[beer3] = 0.0;

	std::map<std::shared_ptr<Commodity>, double> newProfit2;

	newProfit2[meal1] = 0.0;
	newProfit2[meal2] = 0.0;
	newProfit2[meal3] = 0.0;
	newProfit2[beer1] = 0.0;
	newProfit2[beer2] = 0.0;
	newProfit2[beer3] = 0.0;

	std::map<std::shared_ptr<Commodity>, pAndA> pandaGrads;
	pandaGrads[meal1] = { 1, 1, false };
	pandaGrads[meal2] = { 1, 1, false };
	pandaGrads[meal3] = { 1, 1, false };
	pandaGrads[beer1] = { 1, 1, false };
	pandaGrads[beer2] = { 1, 1, false };
	pandaGrads[beer3] = { 1, 1, false };

	double stepSize = 1.01;

	int count = 0;
	while (true)
	{
		count++;

		solver.OptimizeTimeAndPurchases2(24.0, 0, false, inv, commodityPurchases, activityPurchases);
		//std::cout << "!!! " << blah << std::endl;
		for (auto& panda : pandas)
		{
			double orgSold = commodityPurchases.find(panda.first)->second;
			orgProfit[panda.first] = orgSold*panda.second.price;

			solver.setPriceAndAmount(panda.first, panda.second.price*stepSize, panda.second.amount);
		}
		
		solver.OptimizeTimeAndPurchases2(24.0, 0, false, inv, commodityPurchases, activityPurchases);

		for (auto& panda : pandas)
		{
			double newSold = commodityPurchases.find(panda.first)->second;
			newProfit[panda.first] = newSold*panda.second.price*stepSize;

			solver.setPriceAndAmount(panda.first, panda.second.price, panda.second.amount);

			pandaGrads[panda.first].price = newProfit[panda.first] - orgProfit[panda.first];
		}

		if (count % 100 == 0)
		{
			double spend = 0;
			for (auto& purchase : commodityPurchases)
			{
				if (pandas.find(purchase.first) != pandas.end())
				{
					std::cout << purchase.first->getName() << " : " << purchase.second << " at price: " << pandas[purchase.first].price << "       ........       " << pandas[purchase.first].amount << (pandas[purchase.first].priceLocked ? " Locked" : " ...") << std::endl;
				}
				else
				{
					std::cout << purchase.first->getName() << " : " << purchase.second << " at price: " << 1.0 << std::endl;
				}

				spend += pandas[purchase.first].price * purchase.second;

			}

			std::cout << "SPEND: " << spend << std::endl;

			for (auto& purchase : activityPurchases)
			{
				std::cout << purchase.first->getName() << " : " << purchase.second << std::endl;
			}

			std::cout << "ITERATION: " << count << std::endl;
		}


		double gradLenPrice = 0.0;
		for (auto& grad : pandaGrads)
		{
			/*if (abs(grad.second.price) < abs(grad.second.amount))
			{
				grad.second.price = grad.second.amount;
			}*/

			gradLenPrice += grad.second.price * grad.second.price;
			
		}

		gradLenPrice = sqrt(gradLenPrice);

		// update solver for next iteration
		
		for (auto& panda : pandas)
		{
			
			pandaGrads[panda.first].price /= gradLenPrice;
				
			if (panda.second.price + pandaGrads[panda.first].price*(stepSize - 1.0) > 1.0 && !panda.second.priceLocked)
			{
				panda.second.price += pandaGrads[panda.first].price*(stepSize - 1.0);
			}
			else
			{
				panda.second.price = 1.0;
			}
		}

		//system("pause");
	}

    
    system("pause");
    return 0;
}





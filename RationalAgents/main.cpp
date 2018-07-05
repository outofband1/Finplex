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

    float money = 80;

    PurchaseSolver solver;

	int count = 0;
	for (auto& commodity : defStore.getCommodityDefinitions())
	{
		solver.registerCommodity(commodity.second);
		switch (count)
		{
		case 0:
			solver.setPriceAndAmount(commodity.second, 1.1f, 3.0f);
			break;
		case 1:
			solver.setPriceAndAmount(commodity.second, 1.2f, 4.0f);
			break;
		case 2:
			solver.setPriceAndAmount(commodity.second, 1.3f, -1.0f);
			break;
		}

		count++;
	}
    
	for (auto& utility : defStore.getUtilityDefinitions())
	{
		solver.registerUtility(utility.second);
	}
    
    // return res
    solver.createSimplexSolver(money);

    std::cout << std::endl;
    system("pause");
    return 0;
}





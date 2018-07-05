#include "Simplex.h"
#include <tchar.h>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "Utility.h"
#include "Commodity.h"
#include "PurchaseSolver.h"

int _tmain(int argc, _TCHAR* argv[])
{
    std::map<std::shared_ptr<Commodity>, std::map<std::shared_ptr<Utility>, float>> commodities;
    std::map<std::shared_ptr<Commodity>, float> prices;
    std::map<std::shared_ptr<Commodity>, float> amounts;

    std::shared_ptr<Commodity> x1 = std::make_shared<Commodity>("Food");
    std::shared_ptr<Commodity> x2 = std::make_shared<Commodity>("Drink");
    std::shared_ptr<Commodity> x3 = std::make_shared<Commodity>("Savings");


    Utility::PieceWiseLinearCurve foodCurve;
    foodCurve.addCurvePiece(2, 0);
    foodCurve.addCurvePiece(1, 2);
    foodCurve.addCurvePiece(0.1f, 4.5f);
    std::shared_ptr<Utility> food = std::make_shared<Utility>("Nutrition", foodCurve);

    Utility::PieceWiseLinearCurve drinkCurve;
    drinkCurve.addCurvePiece(2.0, 0);
    drinkCurve.addCurvePiece(0.1f, 4.5f);
    std::shared_ptr<Utility> drink = std::make_shared<Utility>("Water", drinkCurve);

    Utility::PieceWiseLinearCurve savingsCurve;
    savingsCurve.addCurvePiece(0.3f, 0);
    savingsCurve.addCurvePiece(0.01f, 2);
    std::shared_ptr<Utility> savings = std::make_shared<Utility>("Savings", savingsCurve);

    x1->addUtility(food, 1.0f);
    x1->addUtility(drink, 0.1f);

    commodities[x1][food] = 1.0f;
    commodities[x1][savings] = 0.0f;
    commodities[x1][drink] = 0.1f;
    prices[x1] = 1.1f;
    amounts[x1] = 3.0f;

    x2->addUtility(food, 0.1f);
    x2->addUtility(drink, 1.0f);

    commodities[x2][food] = 0.1f;
    commodities[x2][savings] = 0.0f;
    commodities[x2][drink] = 1.0f;
    prices[x2] = 1.2f;
    amounts[x2] = 4.0f;

    x3->addUtility(savings, 1.0f);

    commodities[x3][food] = 0.0f;
    commodities[x3][savings] = 1.0f;
    commodities[x3][drink] = 0.0f;
    prices[x3] = 1.3f;

    float money = 8;

    PurchaseSolver solver;
    solver.registerCommodity(x1);
    solver.registerCommodity(x2);
    solver.registerCommodity(x3);

    solver.registerUtility(food);
    solver.registerUtility(drink);
    solver.registerUtility(savings);

    solver.setPriceAndAmount(x1, 1.1f, 3.0f);
    solver.setPriceAndAmount(x2, 1.2f, 4.0f);
    solver.setPriceAndAmount(x3, 1.3f, -1.0f);


    // return res
    solver.createSimplexSolver(8);

    std::cout << std::endl;
    system("pause");
    return 0;
}





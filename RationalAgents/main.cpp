#include "Simplex.h"
#include <tchar.h>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "Utility.h"
#include "Commodity.h"





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

    commodities[x1][food] = 1.0f;
    commodities[x1][savings] = 0.0f;
    commodities[x1][drink] = 0.1f;
    prices[x1] = 1.0f;
    amounts[x1] = 3.0f;

    commodities[x2][food] = 0.1f;
    commodities[x2][savings] = 0.0f;
    commodities[x2][drink] = 1.0f;
    prices[x2] = 1.0f;
    amounts[x2] = 3.0f;

    commodities[x3][food] = 0.0f;
    commodities[x3][savings] = 1.0f;
    commodities[x3][drink] = 0.0f;
    prices[x3] = 1.0f;

    float money = 8;


    size_t utilityCount = 0;
    size_t curvePieceCount = 0;
    for (auto& utility : commodities.begin()->second)
    {
        utilityCount++;
        curvePieceCount += utility.first->getCurve().getCurvePieces().size();
    }

    size_t rowCount = 2 + amounts.size() + curvePieceCount; // 2= cost row + spending constraint row

    //                commodities          utilities          constraints - costrow + rhs
    size_t colCount = commodities.size() + utilityCount + rowCount - 1 + 1;

    std::vector<float> tableauData;
    tableauData.resize(colCount * rowCount);

    // setup cost row
    for (size_t i = 0; i < colCount; i++)
    {
        if (i < commodities.size())
        {
            tableauData[i] = 0;
        }
        else if (i < commodities.size() + utilityCount)
        {
            tableauData[i] = -1;
        }
        else
        {
            tableauData[i] = 0;
        }
    }

    size_t colIndex = 0;
    for (auto& commodity : commodities)
    {
        tableauData[(curvePieceCount + 1)*colCount + colIndex] = prices.find(commodity.first)->second;
        tableauData[(curvePieceCount + 1)*colCount + colCount - 1] = money;
        tableauData[(curvePieceCount + 1)*colCount + commodities.size() + utilityCount + curvePieceCount] = 1;
        size_t rowIndex = 1; // row 0 is cost function
        size_t utilityCount = 0;
        size_t pieceCount = 0;
        for (auto& utility : commodity.second)
        {
            float factor = utility.second;
            const Utility::PieceWiseLinearCurve& curve = utility.first->getCurve();

            for (auto& piece : curve.getCurvePieces())
            {
                tableauData[rowIndex * colCount + colIndex] = -factor * piece.getSlope();
                tableauData[rowIndex * colCount + colCount - 1] = piece.getSlopeIntercept();
                tableauData[rowIndex * colCount + commodities.size() + utilityCount] = 1.0;
                tableauData[rowIndex * colCount + commodities.size() + commodity.second.size() + pieceCount] = 1.0;
                pieceCount++;
                rowIndex++;
            }
            utilityCount++;
        }

        colIndex++;
    }

    size_t amountConstraintCount = 0;
    size_t rowindex = curvePieceCount + 1 + 1; // plus 1 for cost function and plus 1 for money constraint
    colIndex = 0;
    for (auto& commodity : commodities)
    {
        const auto& amount = amounts.find(commodity.first);

        if (amount != amounts.end())
        {
            tableauData[(rowindex)* colCount + colIndex] = 1;
            tableauData[(rowindex)*colCount + commodities.size() + utilityCount + curvePieceCount + colIndex + 1] = 1;
            tableauData[(rowindex)*colCount + colCount - 1] = amount->second;
            rowindex++;
            colIndex++;
        }
    }

    SimplexTableau<float> m(rowCount, colCount);

    for (int repeat = 0; repeat < 1; repeat++)
    {
        m.setTableau(tableauData);

        m.printTableau();

        float res;
        std::vector<float> var;

        m.Solve(res, var);

        m.printTableau();

        std::cout << "Result: " << res << std::endl;

        int i = 0;
        for (auto& v : var)
        {
            std::cout << "basic " << i << " = " << v << std::endl;
            i++;
        }
    }

    std::cout << std::endl;
    system("pause");
    return 0;
}





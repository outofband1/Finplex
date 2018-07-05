#include "PurchaseSolver.h"
#include "Commodity.h"
#include "Utility.h"
#include <memory>

PurchaseSolver::PurchaseSolver()
{

}

void PurchaseSolver::registerCommodity(const std::shared_ptr<Commodity>& commodity)
{
    commodities_.push_back(commodity);
}

void PurchaseSolver::registerUtility(const std::shared_ptr<Utility>& utility)
{
    utilities_.push_back(utility);
}

void PurchaseSolver::setPriceAndAmount(const std::shared_ptr<Commodity>& commodity, const float& price, const float& amount)
{
    PriceAndAmount pa;
    pa.amount_ = amount;
    pa.price_ = price;
    amountsAndPrices_[commodity] = pa;
}

void PurchaseSolver::clearAmountsAndPrices()
{
    amountsAndPrices_.clear();
}

void PurchaseSolver::OptimizeTimeAndPurchases(const float& money, std::map<std::shared_ptr<Commodity>, float>& purchases) const
{

    size_t curvePieceCount = 0;
    size_t commodityCount = commodities_.size();
    size_t utilityCount = utilities_.size();
    for (auto& utility : utilities_)
    {
        curvePieceCount += utility->getCurve().getCurvePieces().size();
    }

    size_t amountCount = 0;
    for (auto& priceAndAmount : amountsAndPrices_)
    {
        if (priceAndAmount.second.amount_ >= 0)
        {
            amountCount++;
        }
    }

    size_t rowCount = 2 + amountCount + curvePieceCount; // 2= cost row + spending constraint row

    //                commodities      utilities      constraints - costrow + rhs
    size_t colCount = commodityCount + utilityCount + rowCount - 1 + 1;

    std::vector<float> tableauData;
    tableauData.resize(colCount * rowCount);

    // setup cost row
    for (size_t i = 0; i < colCount; i++)
    {
        if (i < commodityCount)
        {
            tableauData[i] = 0;
        }
        else if (i < commodityCount + utilityCount)
        {
            tableauData[i] = -1;
        }
        else
        {
            tableauData[i] = 0;
        }
    }

    size_t colIndex = 0;
    for (auto& commodity : commodities_)
    {
        tableauData[(curvePieceCount + 1)*colCount + colIndex] = amountsAndPrices_.find(commodity)->second.price_;
        tableauData[(curvePieceCount + 1)*colCount + colCount - 1] = money;
        tableauData[(curvePieceCount + 1)*colCount + commodityCount + utilityCount + curvePieceCount] = 1;
        size_t rowIndex = 1; // row 0 is cost function
        size_t utilityIndex = 0;
        size_t pieceCount = 0;
        for (auto& utility : utilities_)
        {
            float factor = 0.0;
            auto& commodityUtility = commodity->getUtilities().find(utility);
            if (commodityUtility != commodity->getUtilities().end())
            {
                factor = commodityUtility->second;
            }

            const Utility::PieceWiseLinearCurve& curve = utility->getCurve();

            for (auto& piece : curve.getCurvePieces())
            {
                tableauData[rowIndex * colCount + colIndex] = -factor * piece.getSlope();
                tableauData[rowIndex * colCount + colCount - 1] = piece.getSlopeIntercept();
                tableauData[rowIndex * colCount + commodityCount + utilityIndex] = 1.0;
                tableauData[rowIndex * colCount + commodityCount + utilityCount + pieceCount] = 1.0;
                pieceCount++;
                rowIndex++;
            }
            utilityIndex++;
        }

        colIndex++;
    }

    size_t amountConstraintCount = 0;
    size_t rowindex = curvePieceCount + 1 + 1; // plus 1 for cost function and plus 1 for money constraint
    colIndex = 0;
    for (auto& commodity : commodities_)
    {
        const auto& amount = amountsAndPrices_.find(commodity)->second.amount_;

        if (amount >= 0)
        {
            tableauData[(rowindex)* colCount + colIndex] = 1;
            tableauData[(rowindex)*colCount + commodityCount + utilityCount + curvePieceCount + colIndex + 1] = 1;
            tableauData[(rowindex)*colCount + colCount - 1] = amount;
            rowindex++;
        }
        colIndex++;
    }

    SimplexSolver<float> m(rowCount, colCount);

    m.setTableau(tableauData);

    float maxedUtility;
    std::vector<float> basicVariablesValues;

	m.Solve(maxedUtility, basicVariablesValues);

	purchases.clear();
	size_t varIndex = 0;
	for (auto& com : commodities_)
	{
		purchases[com] = basicVariablesValues[varIndex];
		varIndex++;
	}
}
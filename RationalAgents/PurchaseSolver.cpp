#include "PurchaseSolver.h"
#include "UtilitySource.h"
#include "Utility.h"
#include <memory>

#include "Commodity.h"
#include "Activity.h"

PurchaseSolver::PurchaseSolver()
{

}

void PurchaseSolver::registerUtilitySource(const std::shared_ptr<Commodity>& commodity)
{
    commodities_.push_back(commodity);
}

void PurchaseSolver::registerUtilitySource(const std::shared_ptr<Activity>& activity)
{
	activities_.push_back(activity);
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

void PurchaseSolver::OptimizeTimeAndPurchases(const float& money, std::map<std::shared_ptr<UtilitySource>, float>& purchases) const
{

    size_t curvePieceCount = 0;
    size_t utilitySourceCount = commodities_.size() + activities_.size();
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

    size_t rowCount = 3 + amountCount + curvePieceCount; // 2= cost row + spending constraint row + time row

    //                commodities      utilities      constraints - costrow + rhs
    size_t colCount = utilitySourceCount + utilityCount + rowCount - 1 + 1;

    std::vector<double> tableauData;
    tableauData.resize(colCount * rowCount);

    // setup cost row
    for (size_t i = 0; i < colCount; i++)
    {
        if (i < utilitySourceCount)
        {
            tableauData[i] = 0;
        }
        else if (i < utilitySourceCount + utilityCount)
        {
            tableauData[i] = -1;
        }
        else
        {
            tableauData[i] = 0;
        }
    }

	std::map<size_t, std::shared_ptr<UtilitySource>> utilitySourceCol;

    size_t colIndex = 0;
    for (auto& commodity : commodities_)
    {
		utilitySourceCol[colIndex] = commodity;
		tableauData[(curvePieceCount + 1)*colCount + colIndex] = amountsAndPrices_.find(commodity)->second.price_;
        tableauData[(curvePieceCount + 1)*colCount + colCount - 1] = 0;
        tableauData[(curvePieceCount + 1)*colCount + utilitySourceCount + utilityCount + curvePieceCount] = 1;
        size_t rowIndex = 1; // row 0 is cost function
        size_t utilityIndex = 0;
        size_t pieceCount = 0;
        for (auto& utility : utilities_)
        {
            float factor = 0.0;
			auto& utilitySourceUtility = commodity->getUtilities().find(utility);
			if (utilitySourceUtility != commodity->getUtilities().end())
            {
                factor = utilitySourceUtility->second;
            }

            const Utility::PieceWiseLinearCurve& curve = utility->getCurve();

            for (auto& piece : curve.getCurvePieces())
            {
                tableauData[rowIndex * colCount + colIndex] = -factor * piece.getSlope();
                tableauData[rowIndex * colCount + colCount - 1] = piece.getSlopeIntercept();
                tableauData[rowIndex * colCount + utilitySourceCount + utilityIndex] = 1.0;
                pieceCount++;
                rowIndex++;
            }
            utilityIndex++;
        }

        colIndex++;
    }

	for (auto& activity : activities_)
	{
		utilitySourceCol[colIndex] = activity;
		tableauData[(curvePieceCount + 2)*colCount + colIndex] = 1;
		tableauData[(curvePieceCount + 2)*colCount + colCount - 1] = 24;
		tableauData[(curvePieceCount + 2)*colCount + utilitySourceCount + utilityCount + curvePieceCount] = 1;

		if (activity->getName() == "Labor")
		tableauData[(curvePieceCount + 1)*colCount + colIndex] = -1;
		

		size_t rowIndex = 1; // row 0 is cost function
		size_t utilityIndex = 0;
		size_t pieceCount = 0;
		for (auto& utility : utilities_)
		{
			float factor = 0.0;
			auto& utilitySourceUtility = activity->getUtilities().find(utility);
			if (utilitySourceUtility != activity->getUtilities().end())
			{
				factor = utilitySourceUtility->second;
			}

			const Utility::PieceWiseLinearCurve& curve = utility->getCurve();

			for (auto& piece : curve.getCurvePieces())
			{
				tableauData[rowIndex * colCount + colIndex] = -factor * piece.getSlope();
				tableauData[rowIndex * colCount + colCount - 1] = piece.getSlopeIntercept();
				pieceCount++;
				rowIndex++;
			}
			utilityIndex++;
		}

		colIndex++;
	}

	size_t rowIndex = 1; // row 0 is cost function
	for (size_t col = utilitySourceCount + utilityCount; col < colCount-1; col++)
	{
		tableauData[rowIndex * colCount + col] = 1.0;
		rowIndex++;
	}

    size_t amountConstraintCount = 0;
    size_t rowindex = curvePieceCount + 1 + 1 + 1; // plus 1 for cost function and plus 1 for money constraint
    size_t colIndexutilitySource = 0;
    size_t colIndexConstraint = 0;
	for (auto& commodity : commodities_)
    {
		const auto& amount = amountsAndPrices_.find(commodity)->second.amount_;

        if (amount >= 0)
        {
            tableauData[(rowindex)* colCount + colIndexutilitySource] = 1;
            tableauData[(rowindex)*colCount + colCount - 1] = amount;
            rowindex++;
            colIndexConstraint++;
        }
        colIndexutilitySource++;
    }

    SimplexSolver<double> m(rowCount, colCount);

    m.setTableau(tableauData);

    double maxedUtility;
    std::vector<double> basicVariablesValues;

    m.Solve(maxedUtility, basicVariablesValues);

    purchases.clear();
    size_t varIndex = 0;
    for (auto& comCol : utilitySourceCol)
    {
        purchases[comCol.second] = basicVariablesValues[comCol.first];

    }
}
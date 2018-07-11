#include "PurchaseSolver.h"
#include "UtilitySource.h"
#include "Utility.h"
#include <memory>

#include "Simplex.h"
#include "Commodity.h"
#include "Activity.h"
#include "Inventory.h"

PurchaseSolver::PurchaseSolver()
{

}

void PurchaseSolver::registerUtilitySource(const std::shared_ptr<Commodity>& commodity)
{
    //commodities_.push_back(commodity);
}

void PurchaseSolver::registerUtilitySource(const std::shared_ptr<Activity>& activity)
{
    //activities_.push_back(activity);
}

void PurchaseSolver::registerUtility(const std::shared_ptr<Utility>& utility)
{
    utilities_.push_back(utility);
}

void PurchaseSolver::setPriceAndAmount(const std::shared_ptr<Commodity>& utilitySource, const double& price, const double& amount)
{
    PriceAndAmount pa;
    pa.amount_ = amount;
    pa.price_ = price;
    commodityAmountsAndPrices_[utilitySource] = pa;
}

void PurchaseSolver::setPriceAndAmount(const std::shared_ptr<Activity>& utilitySource, const double& price, const double& amount)
{
    PriceAndAmount pa;
    pa.amount_ = amount;
    pa.price_ = price;
    activityAmountsAndPrices_[utilitySource] = pa;
}

void PurchaseSolver::clearAmountsAndPrices()
{
    commodityAmountsAndPrices_.clear();
    activityAmountsAndPrices_.clear();
}

void PurchaseSolver::removeUtilitySource(const std::shared_ptr<Commodity>& commodity)
{
    auto& com = commodityAmountsAndPrices_.find(commodity);
    if (com != commodityAmountsAndPrices_.end())
    {
        commodityAmountsAndPrices_.erase(com);
    }
}

void PurchaseSolver::removeUtilitySource(const std::shared_ptr<Activity>& activity)
{
    auto& act = activityAmountsAndPrices_.find(activity);
    if (act != activityAmountsAndPrices_.end())
    {
        activityAmountsAndPrices_.erase(act);
    }
}

void PurchaseSolver::OptimizeTimeAndPurchases(const double& time, const double& savings, const Inventory& inventory, std::map<std::shared_ptr<Commodity>, double>& commodityPurchases, std::map<std::shared_ptr<Activity>, double>& activityPurchases) const
{

    size_t curvePieceCount = 0;
    size_t utilitySourceCount = commodityAmountsAndPrices_.size() + activityAmountsAndPrices_.size();
    size_t utilityCount = utilities_.size();
    for (auto& utility : utilities_)
    {
        curvePieceCount += utility->getCurve().getCurvePieces().size();
    }

    size_t amountCount = 0;
    for (auto& priceAndAmount : commodityAmountsAndPrices_)
    {
        if (priceAndAmount.second.amount_ >= 0)
        {
            amountCount++;
        }
    }

    for (auto& priceAndAmount : activityAmountsAndPrices_)
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

    std::map<size_t, std::shared_ptr<Commodity>> commodityCol;
    std::map<size_t, std::shared_ptr<Activity>> activityCol;

    size_t rowIndex = 1; // row 0 is cost function
    size_t utilityIndex = 0;
    size_t pieceCount = 0;
    for (auto& utility : utilities_)
    {
        const Utility::PieceWiseLinearCurve& curve = utility->getCurve();

        for (auto& piece : curve.getCurvePieces())
        {
            tableauData[rowIndex * colCount + colCount - 1] = piece.getSlopeIntercept();
            tableauData[rowIndex * colCount + utilitySourceCount + utilityIndex] = 1.0;
            pieceCount++;
            rowIndex++;
        }
        utilityIndex++;
    }

    size_t colIndex = 0;
    for (auto& commodity : commodityAmountsAndPrices_)
    {
        commodityCol[colIndex] = commodity.first;

        tableauData[(curvePieceCount + 1)*colCount + colIndex] = commodity.second.price_;

        tableauData[(curvePieceCount + 1)*colCount + colCount - 1] = savings;
        tableauData[(curvePieceCount + 1)*colCount + utilitySourceCount + utilityCount + curvePieceCount] = 1;
        size_t rowIndex = 1; // row 0 is cost function
        size_t utilityIndex = 0;
        size_t pieceCount = 0;
        for (auto& utility : utilities_)
        {
            double factor = 0.0;
            auto& utilitySourceUtility = commodity.first->getUtilities().find(utility);
            if (utilitySourceUtility != commodity.first->getUtilities().end())
            {
                factor = utilitySourceUtility->second;
            }

            const Utility::PieceWiseLinearCurve& curve = utility->getCurve();

            double currentAmount = inventory.getAmount(commodity.first);

            for (auto& piece : curve.getCurvePieces())
            {
                tableauData[rowIndex * colCount + colIndex] = -factor * piece.getSlope();
                tableauData[rowIndex * colCount + colCount - 1] += piece.getSlope() * currentAmount * factor;
                pieceCount++;
                rowIndex++;
            }
            utilityIndex++;
        }

        colIndex++;
    }

    for (auto& activity : activityAmountsAndPrices_)
    {
        activityCol[colIndex] = activity.first;


        tableauData[(curvePieceCount + 1)*colCount + colIndex] = activity.second.price_;

        tableauData[(curvePieceCount + 2)*colCount + colIndex] = 1;
        tableauData[(curvePieceCount + 2)*colCount + colCount - 1] = time;
        //tableauData[(curvePieceCount + 2)*colCount + utilitySourceCount + utilityCount + curvePieceCount] = 4;


        size_t rowIndex = 1; // row 0 is cost function
        size_t utilityIndex = 0;
        size_t pieceCount = 0;
        for (auto& utility : utilities_)
        {
            double factor = 0.0;
            auto& utilitySourceUtility = activity.first->getUtilities().find(utility);
            if (utilitySourceUtility != activity.first->getUtilities().end())
            {
                factor = utilitySourceUtility->second;
            }

            const Utility::PieceWiseLinearCurve& curve = utility->getCurve();

            for (auto& piece : curve.getCurvePieces())
            {
                tableauData[rowIndex * colCount + colIndex] = -factor * piece.getSlope();
                pieceCount++;
                rowIndex++;
            }
            utilityIndex++;
        }

        colIndex++;
    }

    rowIndex = 1; // row 0 is cost function
    for (size_t col = utilitySourceCount + utilityCount; col < colCount - 1; col++)
    {
        tableauData[rowIndex * colCount + col] = 1.0;
        rowIndex++;
    }

    size_t amountConstraintCount = 0;
    size_t rowindex = curvePieceCount + 1 + 1 + 1; // plus 1 for cost function and plus 1 for money constraint
    size_t colIndexutilitySource = 0;
    size_t colIndexConstraint = 0;
    for (auto& commodity : commodityAmountsAndPrices_)
    {
        if (commodity.second.amount_ >= 0)
        {
            tableauData[(rowindex)* colCount + colIndexutilitySource] = 1;
            tableauData[(rowindex)*colCount + colCount - 1] = commodity.second.amount_;
            rowindex++;
            colIndexConstraint++;
        }
        colIndexutilitySource++;
    }

    for (auto& activity : activityAmountsAndPrices_)
    {

        if (activity.second.amount_ >= 0)
        {
            tableauData[(rowindex)* colCount + colIndexutilitySource] = 1;
            tableauData[(rowindex)*colCount + colCount - 1] = activity.second.amount_;
            rowindex++;
            colIndexConstraint++;
        }
        colIndexutilitySource++;
    }

    SimplexSolver<double> m(rowCount, colCount);

    commodityPurchases.clear();
    activityPurchases.clear();

    m.setTableau(tableauData);

    //m.printTableau();



    double maxedUtility;
    std::vector<double> basicVariablesValues;

    try
    {
        m.Solve(maxedUtility, basicVariablesValues);

        for (auto& comCol : commodityCol)
        {
            if (basicVariablesValues.size() > comCol.first)
            {
                commodityPurchases[comCol.second] = basicVariablesValues[comCol.first];
            }
        }

        for (auto& actCol : activityCol)
        {
            if (basicVariablesValues.size() > actCol.first)
            {
                activityPurchases[actCol.second] = basicVariablesValues[actCol.first];
            }

        }

        if (commodityPurchases.size() == 0)
        {
            m.setTableau(tableauData);
            m.printTableau();
        }

    }
    catch (...)
    {
        m.setTableau(tableauData);
        m.printTableau();
        std::cout << "uh oh" << std::endl;
    }

}
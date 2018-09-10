#include "PurchaseSolver.h"
#include "UtilitySource.h"
#include "Utility.h"
#include <memory>

#include "Simplex.h"
#include "Commodity.h"
#include "Activity.h"
#include "Inventory.h"
#include <string.h>

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

            double currentAmount = 0.0; // inventory.getAmount(commodity.first);

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

    m.printTableau();



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


#include "lp_lib.h"

double PurchaseSolver::OptimizeTimeAndPurchases2(const double& time, const double& savings, const bool& laborConstraints, const Inventory& inventory, std::map<std::shared_ptr<Commodity>, double>& commodityPurchases, std::map<std::shared_ptr<Activity>, double>& activityPurchases) const
{
    commodityPurchases.clear();
    activityPurchases.clear();

    std::vector<std::shared_ptr<Commodity>> comOrderAdded;
    std::vector<std::shared_ptr<Activity>> actOrderAdded;
    int constraintCount = 0;

    size_t utilitySourceCount = commodityAmountsAndPrices_.size() + activityAmountsAndPrices_.size();
    size_t utilityCount = utilities_.size();

    int objectiveCount = utilitySourceCount + utilityCount;

    lprec *lp;

    if ((lp = make_lp(0, objectiveCount)) == nullptr)
    {
        throw;
    }

    set_verbose(lp, NEUTRAL);

    std::map < std::pair<std::shared_ptr<Utility>, int>, std::vector<double>> vals;
    std::map < std::pair<std::shared_ptr<Utility>, int>, std::vector<int>> cols;

    std::vector<int> timeConstraintCols;
    std::vector<double> timeConstraintVals;

    std::vector<int> moneyConstraintCols;
    std::vector<double> moneyConstraintVals;

    std::vector<int> laborConstraintCols;
    std::vector<double> laborConstraintVals;

    set_maxim(lp);
    char name[256];
    int colIndex = 1;
    for (auto& commodity : commodityAmountsAndPrices_)
    {
        const double& amount = commodity.second.amount_;

        strncpy_s(name, commodity.first->getName().c_str(), 255);
        set_col_name(lp, colIndex, name);

        comOrderAdded.push_back(commodity.first);

        if (amount >= 0)
        {
            set_bounds(lp, colIndex, 0.0, amount);
        }

        for (auto& comUtil : commodity.first->getUtilities())
        {
            int pieceIndex = 0;
            for (auto& curvePiece : comUtil.first->getCurve().getCurvePieces())
            {
                if (abs(-curvePiece.getSlope() * comUtil.second) > 10E-6)
                {
                    std::pair<std::shared_ptr<Utility>, int> p = std::make_pair(comUtil.first, pieceIndex);
                    auto& it = cols.find(p);
                    if (it == cols.end())
                    {
                        cols[p] = std::vector<int>();
                        vals[p] = std::vector<double>();
                    }

                    cols[p].push_back(colIndex);
                    vals[p].push_back(-curvePiece.getSlope() * comUtil.second);
                }

                pieceIndex++;
            }
        }

        if (abs(commodity.second.price_) > 10E-6)
        {
            moneyConstraintCols.push_back(colIndex);
            moneyConstraintVals.push_back(commodity.second.price_);
        }

        laborConstraintCols.push_back(colIndex);
        laborConstraintVals.push_back(1.0);

        colIndex++;
    }

    for (auto& activity : activityAmountsAndPrices_)
    {
        const double& amount = activity.second.amount_;

        strncpy_s(name, activity.first->getName().c_str(), 255);
        set_col_name(lp, colIndex, name);

        actOrderAdded.push_back(activity.first);

        if (amount >= 0)
        {
            set_bounds(lp, colIndex, 0.0, amount);
        }

        for (auto& actUtil : activity.first->getUtilities())
        {
            int pieceIndex = 0;
            for (auto& curvePiece : actUtil.first->getCurve().getCurvePieces())
            {
                if (abs(-curvePiece.getSlope() * actUtil.second) > 10E-6)
                {
                    std::pair<std::shared_ptr<Utility>, int> p = std::make_pair(actUtil.first, pieceIndex);
                    auto& it = cols.find(p);
                    if (it == cols.end())
                    {
                        cols[p] = std::vector<int>();
                        vals[p] = std::vector<double>();
                    }

                    cols[p].push_back(colIndex);
                    vals[p].push_back(-curvePiece.getSlope() * actUtil.second);
                }


                pieceIndex++;
            }
        }

        timeConstraintCols.push_back(colIndex);
        timeConstraintVals.push_back(1.0);

        if (abs(activity.second.price_) > 10E-6)
        {
            moneyConstraintCols.push_back(colIndex);
            moneyConstraintVals.push_back(activity.second.price_);
        }

        if (activity.second.price_ < -10E-6)
        {
            laborConstraintCols.push_back(colIndex);
            laborConstraintVals.push_back(-1.0);
        }

        colIndex++;
    }

    for (auto& utility : utilities_)
    {
        strncpy_s(name, utility->getName().c_str(), 255);
        set_col_name(lp, colIndex, name);
        set_obj(lp, colIndex, 1);

        const Utility::PieceWiseLinearCurve& curve = utility->getCurve();

        int pieceIndex = 0;
        for (auto& piece : curve.getCurvePieces())
        {
            const double& intercept = piece.getSlopeIntercept();
            std::pair<std::shared_ptr<Utility>, int> p = std::make_pair(utility, pieceIndex);

            cols[p].push_back(colIndex);
            vals[p].push_back(1);

            add_constraintex(lp, cols[p].size(), vals[p].data(), cols[p].data(), LE, intercept);
            constraintCount += 1;

            pieceIndex++;
        }

        colIndex++;
    }

    add_constraintex(lp, timeConstraintCols.size(), timeConstraintVals.data(), timeConstraintCols.data(), EQ, time);
    constraintCount += 1;

    if (savings >= 0)
    {
        add_constraintex(lp, moneyConstraintCols.size(), moneyConstraintVals.data(), moneyConstraintCols.data(), LE, savings);
        constraintCount += 1;
    }


    if (laborConstraints)
    {
        add_constraintex(lp, laborConstraintCols.size(), laborConstraintVals.data(), laborConstraintCols.data(), EQ, 0);
        constraintCount += 1;
    }


    print_lp(lp);

    int status = solve(lp);

    if (status != OPTIMAL)
    {
        throw;
    }

    REAL* ptr_pv;
    get_ptr_primal_solution(lp, &ptr_pv);

    for (size_t i = 0; i < comOrderAdded.size(); i++)
    {
        //std::cout << ptr_pv[constraintCount+1+i] << std::endl;
        commodityPurchases.insert(std::make_pair(comOrderAdded[i], ptr_pv[constraintCount + 1 + i]));
    }

    for (size_t i = 0; i < actOrderAdded.size(); i++)
    {
        //std::cout << ptr_pv[comOrderAdded.size()+constraintCount + 1 + i] << std::endl;
        activityPurchases.insert(std::make_pair(actOrderAdded[i], ptr_pv[comOrderAdded.size() + constraintCount + 1 + i]));
    }

    print_solution(lp, 1);

    //print_duals(lp);



    //free_lp(&lp);

    double tmp = ptr_pv[0];


    delete_lp(lp);

    return tmp;
}
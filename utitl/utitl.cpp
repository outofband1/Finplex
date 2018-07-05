#include "Simplex.h"
#include <tchar.h>
#include <iostream>
#include <vector>
#include <memory>
#include <map>


class LinearCurvePiece
{
public:
    LinearCurvePiece(const float& slope, const float& slopeIntercept) :
        a_(slope),
        b_(slopeIntercept)
    {

    }

    const float& getSlope() const
    {
        return a_;
    }

    const float& getSlopeIntercept() const
    {
        return b_;
    }
private:
    float a_, b_;
};

class PieceWiseLinearCurve
{
public:

    void addCurvePiece(const float& slope, const float& slopeIntercept)
    {
        curvePieces_.push_back(LinearCurvePiece(slope, slopeIntercept));
    }

    const std::vector<LinearCurvePiece>& getCurvePieces() const
    {
        return curvePieces_;
    }

private:
    std::vector<LinearCurvePiece> curvePieces_;
};

class Utility
{
public:
    void setCurve(const PieceWiseLinearCurve& curve)
    {
        curve_ = curve;
    }

    const PieceWiseLinearCurve& getCurve() const
    {
        return curve_;
    }
private:


    PieceWiseLinearCurve curve_;
};

class Commodity
{
public:

private:
};

int _tmain(int argc, _TCHAR* argv[])
{
    std::map<std::shared_ptr<Commodity>, std::map<std::shared_ptr<Utility>, float>> commodities;
    std::map<std::shared_ptr<Commodity>, float> prices;

    std::shared_ptr<Commodity> x1 = std::make_shared<Commodity>();
    std::shared_ptr<Commodity> x2 = std::make_shared<Commodity>();

    std::shared_ptr<Utility> stuff = std::make_shared<Utility>();
    PieceWiseLinearCurve stuffCurve;
    stuffCurve.addCurvePiece(2, 0);
    stuffCurve.addCurvePiece(1, 2);
    stuffCurve.addCurvePiece(0.1f, 4.5f);
    stuff->setCurve(stuffCurve);

    std::shared_ptr<Utility> savings = std::make_shared<Utility>();
    PieceWiseLinearCurve savingsCurve;
    savingsCurve.addCurvePiece(0.3f, 0);
    savingsCurve.addCurvePiece(0.0, 2);
    savings->setCurve(savingsCurve);

    commodities[x1][stuff] = 1.0;
    commodities[x1][savings] = 0.0;
    prices[x1] = 1.0;

    commodities[x2][stuff] = 0.0;
    commodities[x2][savings] = 1.0;
    prices[x2] = 1.0;

    std::vector<std::shared_ptr<Utility>> utilities;
    utilities.push_back(stuff);
    utilities.push_back(savings);

    float money = 10;

    size_t rowCount = 2; // cost row + spending constraint row
    for (auto& utility : utilities)
    {
        rowCount += utility->getCurve().getCurvePieces().size();
    }

    //                commodities        utilities          constraints - costrow + rhs
    size_t colCount = commodities.size() + utilities.size() + rowCount - 1 + 1;

    std::vector<float> tableauData;
    tableauData.resize(colCount * rowCount);

    // setup cost row
    for (size_t i = 0; i < colCount; i++)
    {
        if (i < commodities.size())
        {
            tableauData[i] = 0;
        }
        else if (i < commodities.size() + utilities.size())
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
        tableauData[(rowCount - 1)*colCount + colIndex] = prices.find(commodity.first)->second;
        tableauData[(rowCount - 1)*colCount + colCount - 1] = money;
        tableauData[(rowCount - 1)*colCount + colCount - 2] = 1;
        size_t rowIndex = 1; // row 0 is cost function
        size_t utilityCount = 0;
        size_t pieceCount = 0;
        for (auto& utility : commodity.second)
        {
            float factor = utility.second;
            const PieceWiseLinearCurve& curve = utility.first->getCurve();

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

    SimplexTableau<float> m(rowCount, colCount);

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

    std::cout << std::endl;
    system("pause");
    return 0;
}





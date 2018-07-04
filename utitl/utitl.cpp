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

    std::shared_ptr<Commodity> x1 = std::make_shared<Commodity>();
    std::shared_ptr<Commodity> x2 = std::make_shared<Commodity>();

    std::shared_ptr<Utility> stuff = std::make_shared<Utility>();
    PieceWiseLinearCurve stuffCurve;
    stuffCurve.addCurvePiece(2, 0);
    stuffCurve.addCurvePiece(1, 2);
    stuffCurve.addCurvePiece(0.1f, 4.5f);
    stuff->setCurve(stuffCurve);

    commodities[x1][stuff] = 1.0;

    std::shared_ptr<Utility> savings = std::make_shared<Utility>();
    PieceWiseLinearCurve savingsCurve;
    savingsCurve.addCurvePiece(0.3f, 0);
    savingsCurve.addCurvePiece(0.0, 2);
    savings->setCurve(savingsCurve);

    commodities[x2][savings] = 1.0;

    std::vector<std::shared_ptr<Utility>> utilities;
    utilities.push_back(stuff);
    utilities.push_back(savings);



    size_t rowCount = 2; // cost row + spending constraint row
    for (auto& utility : utilities)
    {
        rowCount += utility->getCurve().getCurvePieces().size();
    }

    //                commodities        utilities          constraints - costrow + rhs
    size_t colCount = commodities.size() + utilities.size() + rowCount - 1 + 1;

    float *tableauData = new float[colCount * rowCount];
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

    size_t rowIndex = 1; // row 0 is cost function
    for (auto& utility : utilities)
    {

    }

    SimplexTableau<float, 7, 11> m;

    float data[] =
    {
        +0, +0, -1, -1, +0, +0, +0, +0, +0, +0, +0,
        -2, +0, +1, +0, +1, +0, +0, +0, +0, +0, +0,
        -1, +0, +1, +0, +0, +1, +0, +0, +0, +0, +2,
        -0.1, +0, +1, +0, +0, +0, +1, +0, +0, +0, +4.5,
        -0, -0.3, +0, +1, +0, +0, +0, +1, +0, +0, +0,
        +0, +0, +0, +1, +0, +0, +0, +0, +1, +0, +2,
        +1, +1, +0, +0, +0, +0, +0, +0, +0, +1, +8,
    };

    m.setTableau(data);
    m.print();

    float var[5];
    float res;

    m.Solve(res, var);

    m.print();

    std::cout << "Result: " << res << std::endl;

    for (size_t i = 0; i < 5; i++)
    {
        std::cout << "basic " << i << " = " << var[i] << std::endl;
    }







    std::cout << std::endl;
    system("pause");
    return 0;
}





#pragma once
#include <vector>

#include "Definition.h"

class Utility : public Definition
{
public:
    class LinearCurvePiece
    {
    public:
        LinearCurvePiece(const double& slope, const double& intercept);
        const double& getSlope() const;
        const double& getSlopeIntercept() const;
    private:
        double slope_, intercept_;
    };

    class PieceWiseLinearCurve
    {
    public:
        void addCurvePiece(const double& slope, const double& intercept);
        const std::vector<LinearCurvePiece>& getCurvePieces() const;
    private:
        std::vector<LinearCurvePiece> curvePieces_;
    };

    Utility(const std::string& name, const PieceWiseLinearCurve& curve);

    const PieceWiseLinearCurve& getCurve() const;

private:
    PieceWiseLinearCurve curve_;
};

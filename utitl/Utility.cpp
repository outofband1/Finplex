#include "Utility.h"

Utility::LinearCurvePiece::LinearCurvePiece(const float& slope, const float& intercept) :
    slope_(slope),
    intercept_(intercept)
{}

const float& Utility::LinearCurvePiece::getSlope() const
{
    return slope_;
}

const float& Utility::LinearCurvePiece::getSlopeIntercept() const
{
    return intercept_;
}

void Utility::PieceWiseLinearCurve::addCurvePiece(const float& slope, const float& intercept)
{
    curvePieces_.push_back(LinearCurvePiece(slope, intercept));
}

const std::vector<Utility::LinearCurvePiece>& Utility::PieceWiseLinearCurve::getCurvePieces() const
{
    return curvePieces_;
}

void Utility::setCurve(const Utility::PieceWiseLinearCurve& curve)
{
    curve_ = curve;
}

const Utility::PieceWiseLinearCurve& Utility::getCurve() const
{
    return curve_;
}

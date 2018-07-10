#include "Utility.h"

Utility::LinearCurvePiece::LinearCurvePiece(const double& slope, const double& intercept) :
    slope_(slope),
    intercept_(intercept)
{}

const double& Utility::LinearCurvePiece::getSlope() const
{
    return slope_;
}

const double& Utility::LinearCurvePiece::getSlopeIntercept() const
{
    return intercept_;
}

void Utility::PieceWiseLinearCurve::addCurvePiece(const double& slope, const double& intercept)
{
    curvePieces_.push_back(LinearCurvePiece(slope, intercept));
}

const std::vector<Utility::LinearCurvePiece>& Utility::PieceWiseLinearCurve::getCurvePieces() const
{
    return curvePieces_;
}

Utility::Utility(const std::string& name, const PieceWiseLinearCurve& curve) :
    Definition(name),
    curve_(curve)
{

}

const Utility::PieceWiseLinearCurve& Utility::getCurve() const
{
    return curve_;
}
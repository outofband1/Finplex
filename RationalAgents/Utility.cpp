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

Utility::Utility(const std::string name, const PieceWiseLinearCurve& curve) :
    name_(name),
    curve_(curve)
{

}

void Utility::setDescription(const std::string description)
{
    description_ = description;
}

const Utility::PieceWiseLinearCurve& Utility::getCurve() const
{
    return curve_;
}

const std::string& Utility::getName() const
{
    return name_;
}

const std::string& Utility::getDescription() const
{
    return description_;
}


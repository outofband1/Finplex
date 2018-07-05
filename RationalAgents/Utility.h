#pragma once
#include <vector>

class Utility
{
public:
    class LinearCurvePiece
    {
    public:
        LinearCurvePiece(const float& slope, const float& intercept);
        const float& getSlope() const;
        const float& getSlopeIntercept() const;
    private:
        float slope_, intercept_;
    };

    class PieceWiseLinearCurve
    {
    public:
        void addCurvePiece(const float& slope, const float& intercept);
        const std::vector<LinearCurvePiece>& getCurvePieces() const;
    private:
        std::vector<LinearCurvePiece> curvePieces_;
    };

    Utility(const std::string name, const PieceWiseLinearCurve& curve);

    void setDescription(const std::string description);

    const PieceWiseLinearCurve& getCurve() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
private:
    std::string name_;
    std::string description_;

    PieceWiseLinearCurve curve_;
};

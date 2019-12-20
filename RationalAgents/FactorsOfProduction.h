#pragma once
#include <vector>
#include <memory>

class CommodityDefinition;

class FactorsOfProduction
{
public:
    struct Input
    {
        std::shared_ptr<CommodityDefinition> commodity_;
        double amount_;
    };

    FactorsOfProduction();

    void setLabout(const double labour);
    double getLabour() const;

    void addInput(const std::shared_ptr<CommodityDefinition>& commodity, const double amount);
    const std::vector<Input>& getInputs() const;

private:
    double labour_;
    std::vector<Input> inputs_;


    // min labour skill
    // sub goods
};
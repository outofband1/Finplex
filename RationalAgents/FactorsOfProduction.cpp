#include "FactorsOfProduction.h"

FactorsOfProduction::FactorsOfProduction()
{
    labour_ = 0.0;
}

void FactorsOfProduction::setLabout(const double labour)
{
    labour_ = labour;
}

double FactorsOfProduction::getLabour() const
{
    return labour_;
}

void FactorsOfProduction::addInput(const std::shared_ptr<CommodityDefinition>& commodity, const double amount)
{
    Input i;
    i.commodity_ = commodity;
    i.amount_ = amount;
    inputs_.push_back(i);
}

const std::vector<FactorsOfProduction::Input>& FactorsOfProduction::getInputs() const
{
    return inputs_;
}

#include "CommodityProducer.h"
#include "CommodityInstance.h"
#include "Commodity.h"


CommodityProducer::CommodityProducer(const std::string& name, const std::shared_ptr<Commodity>& commodity) :
    Producer(name)
{

    std::string cName = name + "'s " + commodity->getName();

    commodityInstance_ = std::make_shared<CommodityInstance>(cName, commodity);
}

const std::shared_ptr<CommodityInstance>& CommodityProducer::getCommodityInstance() const
{
    return commodityInstance_;
}

double CommodityProducer::getCapacity() const
{
    const FactorsOfProduction& fop = commodityInstance_->getCommodity()->getFactorsOfProduction();

    double workingHours = 8.0;
    return workingHours / fop.getLabour();
}

double CommodityProducer::getMinimumSellingPrice() const
{
    const FactorsOfProduction& fop = commodityInstance_->getCommodity()->getFactorsOfProduction();

    // call market to get total fop price (maybe as a moving average)
    double hourlyWage = 10.0;
    return fop.getLabour() * hourlyWage;
}
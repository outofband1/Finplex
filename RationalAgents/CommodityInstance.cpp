#include "CommodityInstance.h"

CommodityInstance::CommodityInstance(const std::string& name, const std::shared_ptr<Commodity>& commodity)
    : commodity_(commodity)
{
    name_ = name;
}

const std::shared_ptr<Commodity>& CommodityInstance::getCommodity() const
{
    return commodity_;
}

const std::string& CommodityInstance::getName() const
{
    return name_;
}
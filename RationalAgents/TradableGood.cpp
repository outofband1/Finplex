#include "TradableGood.h"

TradableGood::TradableGood(const std::shared_ptr<GenericCommodity> commodity, const std::shared_ptr<Entity> owner) :
    commodity_(commodity),
    owner_(owner)
{

}

TradableGood::TradableGood(const std::shared_ptr<GenericCommodity> commodity, const std::shared_ptr<Entity> owner, const double& amount) :
    commodity_(commodity),
    owner_(owner)
{

}

const std::shared_ptr<GenericCommodity> TradableGood::getCommodity() const
{
    return commodity_;
}

const std::shared_ptr<Entity> TradableGood::getOwner() const
{
    return owner_;
}
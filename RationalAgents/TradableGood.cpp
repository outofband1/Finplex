#include "TradableGood.h"

TradableGood::TradableGood(const std::shared_ptr<Commodity>& commodity, Entity* owner) :
    ownership_(owner),
    commodity_(commodity)
{

}

const std::shared_ptr<Commodity>& TradableGood::getCommodity() const
{
    return commodity_;
}

Entity* TradableGood::getOwner()
{
    return ownership_;
}

void TradableGood::transferOwnership(const std::shared_ptr<Entity>& newOwner)
{
    ownership_ = newOwner.get();
}
#pragma once
#include <memory>
#include "CommodityInstance.h"
#include "Entity.h"
#include <tuple>

class TradableGood
{
public:
    TradableGood(const std::shared_ptr<GenericCommodity> commodity, const std::shared_ptr<Entity> owner);

    TradableGood(const std::shared_ptr<GenericCommodity> commodity, const std::shared_ptr<Entity> owner, const double& amount);

    const std::shared_ptr<GenericCommodity> getCommodity() const;

    const std::shared_ptr<Entity> getOwner() const;

private:
    std::shared_ptr<GenericCommodity> commodity_;
    std::shared_ptr<Entity> owner_;
};

inline bool operator==(const TradableGood& lhs, const TradableGood& rhs)
{
    return lhs.getCommodity() == rhs.getCommodity() && lhs.getOwner() == rhs.getOwner();
}

inline bool operator <(const TradableGood& x, const TradableGood& y)
{
    return std::tie(x.getCommodity(), x.getOwner()) < std::tie(y.getCommodity(), y.getOwner());
}
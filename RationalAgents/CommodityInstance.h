#pragma once
#include "Commodity.h"

class CommodityInstance
{
public:

    CommodityInstance(const std::string& name, const std::shared_ptr<Commodity>& commodity);

    const std::shared_ptr<Commodity>& getCommodity() const;
    const std::string& getName() const;

private:
    const std::shared_ptr<Commodity> commodity_;
    std::string name_;
};
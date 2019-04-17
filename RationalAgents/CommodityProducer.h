#pragma once

#include "Producer.h"
#include <string>
#include <memory>

class Commodity;
class CommodityInstance;

class CommodityProducer : public Producer
{
public:
    CommodityProducer(const std::string& name, const std::shared_ptr<Commodity>& commodity);

    const std::shared_ptr<CommodityInstance>& getCommodityInstance() const;

    double getCapacity() const;
    double getMinimumSellingPrice() const;

private:
    std::shared_ptr<CommodityInstance> commodityInstance_;

    // tmp
    double currentCapacity_ = 1;

};
#pragma once
#include <memory>

class Commodity;
class Entity;

class TradableGood
{
public:
    TradableGood(const std::shared_ptr<Commodity>& commodity, Entity* owner);

    const std::shared_ptr<Commodity>& getCommodity() const;

    Entity* getOwner();
    void transferOwnership(const std::shared_ptr<Entity>& newOwner);

private:
    Entity* ownership_;
    const std::shared_ptr<Commodity> commodity_;
};
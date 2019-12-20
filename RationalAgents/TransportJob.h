#pragma once
#include <memory>

class Inventory;
class TradableGood;

class TransportJob
{
public:
    TransportJob(const TradableGood& tradableGood, const double amount, const std::shared_ptr<Inventory> source, const std::shared_ptr<Inventory> destination) :
        tradableGood_(tradableGood),
        source_(source),
        destination_(destination),
        amount_(amount)
    {

    }

    const TradableGood& getTradableGood() const
    {
        return tradableGood_;
    }

    const std::shared_ptr<Inventory>& getSource() const
    {
        return source_;
    }

    const std::shared_ptr<Inventory>& getDestination() const
    {
        return destination_;
    }

    const double getAmount() const
    {
        return amount_;
    }

private:
    const TradableGood tradableGood_;
    const std::shared_ptr<Inventory> source_;
    const std::shared_ptr<Inventory> destination_;
    const double amount_;

};
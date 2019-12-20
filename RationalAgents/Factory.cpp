#include "Factory.h"
#include "CommodityDefinition.h"
#include "TradableGood.h"
#include <iostream>

Factory::Factory(const std::string& name, const std::shared_ptr<CommodityDefinition>& commodity)
{
    std::string cName = name + "'s " + commodity->getName();

    commodityInstance_ = std::make_shared<GenericCommodity>(cName, commodity);

    productInventory_ = std::make_shared<Inventory>();
    inputInventory_ = std::make_shared<Inventory>();
}

void Factory::setHourlyWage(const double wage)
{
    hourlyWage_ = wage;
}

const double& Factory::getHourlyWage() const
{
    return hourlyWage_;
}

void Factory::setShiftLength(const double shiftLength)
{
    shiftLength_ = shiftLength;
}

const double& Factory::getShiftLength() const
{
    return shiftLength_;
}

void Factory::addEmployee(const std::shared_ptr<Agent>& employee)
{
    employees_.push_back(employee);
}

const std::shared_ptr<GenericCommodity>& Factory::getCommodityInstance() const
{
    return commodityInstance_;
}

double Factory::getCapacity() const
{
    const FactorsOfProduction& fop = commodityInstance_->getCommodityDefinition()->getFactorsOfProduction();

    return getShiftLength() * employees_.size() / fop.getLabour();
}

void Factory::produce(double amount)
{
    auto& owner = getOwner();

    auto& fop = commodityInstance_->getCommodityDefinition()->getFactorsOfProduction();

    auto& invItems = inputInventory_->getItems();

    std::map<TradableGood, double> destruction;

    double capacity = getCapacity();
    if (amount > capacity)
    {
        amount = capacity;
    }

    for (auto& factor : fop.getInputs())
    {
        double needed = amount * factor.amount_;

        double amountInInv = 0.0;
        // TODO(?!?!?? eta hvad tænkte jeg på her?) commodityinstance change to definition for actual commodities
        for (auto& invItem : invItems)
        {
            if (invItem.first.getCommodity()->getCommodityDefinition() == factor.commodity_)
            {
                amountInInv += invItem.second;
            }
        }

        if (amountInInv < needed)
        {
            amount = amountInInv / factor.amount_;
        }



        if (amount < 10E-6)
        {
            break;
        }
    }
    if (amount > 10E-6)
    {
        for (auto& factor : fop.getInputs())
        {
            double needed = amount * factor.amount_;

            bool destroyed = false;
            do
            {
                destroyed = false;
                for (auto& invItem : invItems)
                {
                    if (invItem.first.getCommodity()->getCommodityDefinition() == factor.commodity_)
                    {
                        double amountInInventory = invItem.second;
                        if (needed > 0.0)
                        {
                            double destroyAmount = needed;
                            if (needed > amountInInventory)
                            {
                                destroyAmount = amountInInventory;
                            }

                            inputInventory_->destroy(invItem.first, destroyAmount);
                            needed -= amountInInventory;
                            destroyed = true;
                            break;
                        }

                        if (needed < 0.0)
                        {
                            break;
                        }
                    }
                }
            }
            while (destroyed && needed > 0.0);
        }

        std::cout << "Produced: " << amount << " " << commodityInstance_->getName() << std::endl;
        productInventory_->create(TradableGood(commodityInstance_, owner), amount);
    }
}
#pragma once

#include "Company.h"
#include <string>
#include <memory>
#include "Inventory.h"
#include "Facility.h"
#include "CommodityInstance.h"

class Factory : public Facility, public std::enable_shared_from_this<Factory>
{
public:
    Factory(const std::string& name, const std::shared_ptr<CommodityDefinition>& commodity);

    const std::shared_ptr<GenericCommodity>& getCommodityInstance() const;

    double getCapacity() const;

    void setHourlyWage(const double wage);
    const double& getHourlyWage() const;

    void setShiftLength(const double shiftLength);
    const double& getShiftLength() const;

    void produce(double amount);

    void addEmployee(const std::shared_ptr<Agent>& employee);

    void payEmployees()
    {
        for (auto& employee : employees_)
        {
            employee->money_ += 0.9 * hourlyWage_ * shiftLength_;

            money_ -= hourlyWage_ * shiftLength_;
        }
    }

    virtual void transferOwnership(const std::shared_ptr<Company>& newOwner)
    {
        if (getOwner() != nullptr)
        {
            getOwner()->removeOwnerShip(shared_from_this());
        }
        newOwner->setOwnerShip(shared_from_this());
        setOwner(newOwner);
    }

    std::vector<std::shared_ptr<Agent>> employees_;

    double money_;

    std::shared_ptr<Inventory> productInventory_;
    std::shared_ptr<Inventory> inputInventory_;
private:
    std::shared_ptr<GenericCommodity> commodityInstance_;

    double hourlyWage_;
    double shiftLength_;

    double clearingAmount_;
    double clearingPrice_;

};
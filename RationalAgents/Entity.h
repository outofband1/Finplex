#pragma once
#include <string>
#include "Inventory.h"

class Entity
{
public:
    Entity(const std::string& name);

    const std::string& getName() const;

    Inventory& getInventory();

    const double& getCurrency();
    void addCurrency(const double& amount);
    void removeCurrency(const double& amount);

    void transferCurrency(std::shared_ptr<Entity>& target, const double& amount);


private:
    std::string name_;
    Inventory inventory_;
    double currency_;
};


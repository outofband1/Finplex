#pragma once

#include "memory"
#include <map>

class Commodity;

class Inventory
{
public:
    void add(const std::shared_ptr<Commodity>& commodity, const double& amount);
    void remove(const std::shared_ptr<Commodity>& commodity, const double& amount);

    double getAmount(const std::shared_ptr<Commodity>& commodity) const;

    void transferTo(Inventory& inventory, const std::shared_ptr<Commodity>& commodity, const double& amount);
private:
    std::map<std::shared_ptr<Commodity>, double> inventory_;
};

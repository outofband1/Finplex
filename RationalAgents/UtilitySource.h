#pragma once
#include <string>
#include <map>
#include <memory>

#include "Definition.h"

class Utility;

class UtilitySource : public Definition
{
public:
    UtilitySource(const std::string& name);

    void addUtility(const std::shared_ptr<Utility>& utility, const double& amount);

    const std::map<std::shared_ptr<Utility>, double>& getUtilities() const;

private:

    std::map<std::shared_ptr<Utility>, double> utilities_;
};
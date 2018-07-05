#pragma once
#include <string>
#include <map>
#include <memory>

class Utility;

class Commodity
{
public:
    Commodity(const std::string name);

    void setDescription(const std::string description);

    void addUtility(const std::shared_ptr<Utility>& utility, const float& amount);

    const std::string& getName() const;
    const std::string& getDescription() const;

    const std::map<std::shared_ptr<Utility>, float>& getUtilities() const;

private:
    std::string name_;
    std::string description_;

    std::map<std::shared_ptr<Utility>, float> utilities_;
};
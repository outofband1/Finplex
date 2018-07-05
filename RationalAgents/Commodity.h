#pragma once
#include <string>

class Commodity
{
public:
    Commodity(const std::string name);

    void setDescription(const std::string description);

    const std::string& getName() const;
    const std::string& getDescription() const;

private:
    std::string name_;
    std::string description_;
};
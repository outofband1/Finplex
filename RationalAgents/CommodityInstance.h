#pragma once
#include <string>


/*
Instantiation of generic commodity(definition)
Holds information like producer (interesting to see), flavour-name etc.
*/

template<typename Definition>
class CommodityInstance
{
public:
    CommodityInstance(const std::string& name, std::shared_ptr<Definition> definition) :
        name_(name),
        commodityDefinition_(definition)
    {

    }

    const std::string& getName() const
    {
        return name_;
    }

    const std::shared_ptr<Definition>& getCommodityDefinition() const
    {
        return commodityDefinition_;
    }

private:
    std::string name_;
    std::shared_ptr<Definition> commodityDefinition_;
};

class CommodityDefinition;

typedef CommodityInstance<CommodityDefinition> GenericCommodity;
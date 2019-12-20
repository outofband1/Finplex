#pragma once

#include <map>
#include <memory>
#include "tinyxml2.h"

class Utility;
class CommodityDefinition;
class Activity;

class DefinitionStorage
{
public:
    void readDefinitions();

    const std::shared_ptr<CommodityDefinition>& getConsumerGood(const std::string& id) const;
    const std::shared_ptr<CommodityDefinition>& getCommodity(const std::string& id) const;

    //const std::map<std::string, std::shared_ptr<Activity>>& getActivityDefinitions() const;
    const std::map<std::string, std::shared_ptr<Utility>>& getUtilityDefinitions() const;
    const std::map<std::string, std::shared_ptr<CommodityDefinition>>& getConsumerGoodDefinitions() const;
private:
    //const std::string activityDefFilename_ = "def_activity.xml";
    const std::string utilityDefFilename_ = "C:/Projects/finplex/RationalAgents/RationalAgents/def_utility.xml";
    const std::string commodityDefFilename_ = "C:/Projects/finplex/RationalAgents/RationalAgents/def_commodities.xml";
    const std::string consumerGoodDefFilename_ = "C:/Projects/finplex/RationalAgents/RationalAgents/def_consumerGoods.xml";
    const std::string fopFilename_ = "C:/Projects/finplex/RationalAgents/RationalAgents/def_fop.xml";

    //void readActivityDefs();
    void readUtilityDefs();
    void readConsumerGoodDefs();
    void readCommodityDefs();
    void readFactorsOfProduction();

    void readGoodUtils(std::shared_ptr<CommodityDefinition> commodity, tinyxml2::XMLElement* utilities);

    //std::map<std::string, std::shared_ptr<Activity>> activityDefs_;
    std::map<std::string, std::shared_ptr<Utility>> utilityDefs_;
    std::map<std::string, std::shared_ptr<CommodityDefinition>> consumerGoodDefs_;
    std::map<std::string, std::shared_ptr<CommodityDefinition>> commodityDefs_;

};
#include "DefinitionStorage.h"
#include "tinyxml2.h"
#include "Utility.h"
#include "Exceptions.h"
#include <windows.h>
#include "FactorsOfProduction.h"
#include "CommodityDefinition.h"
#include <memory>

void DefinitionStorage::readDefinitions()
{
    try
    {
        readUtilityDefs();
        //readActivityDefs(); // depends on utility defs
        readCommodityDefs();
        readConsumerGoodDefs(); // depends on utility defs
        readFactorsOfProduction();
    }
    catch (const Exceptions::OneMessage& e)
    {
        e.print(std::cout);
    }
}

const std::shared_ptr<CommodityDefinition>& DefinitionStorage::getConsumerGood(const std::string& id) const
{
    auto& good = consumerGoodDefs_.find(id);

    if (good != consumerGoodDefs_.end())
    {
        return good->second;
    }
    else
    {
        PITCH(Exceptions::OneMessage, "Consumer good not found");
    }
}

const std::shared_ptr<CommodityDefinition>& DefinitionStorage::getCommodity(const std::string& id) const
{
    auto& commodity = commodityDefs_.find(id);

    if (commodity != commodityDefs_.end())
    {
        return commodity->second;
    }
    else
    {
        PITCH(Exceptions::OneMessage, "Commodity not found");
    }
}


/*const std::map<std::string, std::shared_ptr<Activity>>& DefinitionStorage::getActivityDefinitions() const
{
    return activityDefs_;
}*/

const std::map<std::string, std::shared_ptr<Utility>>& DefinitionStorage::getUtilityDefinitions() const
{
    return utilityDefs_;
}

const std::map<std::string, std::shared_ptr<CommodityDefinition>>& DefinitionStorage::getConsumerGoodDefinitions() const
{
    return consumerGoodDefs_;
}

void DefinitionStorage::readUtilityDefs()
{
    tinyxml2::XMLDocument utilityDoc;
    utilityDoc.LoadFile(utilityDefFilename_.c_str());

    tinyxml2::XMLElement* main = utilityDoc.FirstChildElement("utilities");

    if (main)
    {
        tinyxml2::XMLElement* utility = main->FirstChildElement("utility");
        while (utility)
        {
            std::string id = utility->FirstChildElement("id")->GetText();
            std::string name = utility->FirstChildElement("name")->GetText();
            std::string description = utility->FirstChildElement("description")->GetText();

            tinyxml2::XMLElement* curve = utility->FirstChildElement("curve");

            double maximum = std::atof(curve->FirstChildElement("maximum")->GetText());
            double saturation = std::atof(curve->FirstChildElement("saturation")->GetText());
            double degree = std::atof(curve->FirstChildElement("degree")->GetText());

            std::shared_ptr<Utility> util = std::make_shared<Utility>(name, description);
            util->setCurve(maximum, saturation, degree);

            utilityDefs_[id] = util;

            utility = utility->NextSiblingElement();
        }
    }
    else
    {
        PITCH(Exceptions::OneMessage, "Utility definitions not found");
    }
}

void DefinitionStorage::readConsumerGoodDefs()
{

    tinyxml2::XMLDocument commodityDoc;
    commodityDoc.LoadFile(consumerGoodDefFilename_.c_str());

    tinyxml2::XMLElement* main = commodityDoc.FirstChildElement("consumerGoods");

    if (main)
    {
        tinyxml2::XMLElement* good = main->FirstChildElement("good");
        while (good)
        {
            std::string id = good->FirstChildElement("id")->GetText();
            std::string name = good->FirstChildElement("name")->GetText();
            std::string description = good->FirstChildElement("description")->GetText();
            std::string isActivityString = good->FirstChildElement("activity")->GetText();

            /*tinyxml2::XMLElement* production = good->FirstChildElement("production");
            double labour = 0.0;
            if (production)
            {
                labour = atof(production->FirstChildElement("labour")->GetText());
            }
            FactorsOfProduction productionInput(labour);*/

            bool isActivity = false;

            if (isActivityString == "true")
            {
                isActivity = true;
            }

            std::shared_ptr<CommodityDefinition> com = std::make_shared<CommodityDefinition>(name, description, isActivity);

            tinyxml2::XMLElement* utilities = good->FirstChildElement("utilities");
            if (utilities)
            {
                readGoodUtils(com, utilities);
            }

            consumerGoodDefs_[id] = com;

            good = good->NextSiblingElement();
        }
    }
    else
    {
        PITCH(Exceptions::OneMessage, "Commodity definitio0ns not found");
    }
}

void DefinitionStorage::readCommodityDefs()
{

    tinyxml2::XMLDocument commodityDoc;
    commodityDoc.LoadFile(commodityDefFilename_.c_str());

    tinyxml2::XMLElement* main = commodityDoc.FirstChildElement("commodities");

    if (main)
    {
        tinyxml2::XMLElement* commodity = main->FirstChildElement("commodity");
        while (commodity)
        {
            std::string id = commodity->FirstChildElement("id")->GetText();
            std::string name = commodity->FirstChildElement("name")->GetText();
            std::string description = commodity->FirstChildElement("description")->GetText();

            std::shared_ptr<CommodityDefinition> com = std::make_shared<CommodityDefinition>(name, description, false);

            commodityDefs_[id] = com;

            commodity = commodity->NextSiblingElement();
        }
    }
    else
    {
        PITCH(Exceptions::OneMessage, "Commodity definitio0ns not found");
    }
}

void DefinitionStorage::readFactorsOfProduction()
{

    tinyxml2::XMLDocument fopDoc;
    fopDoc.LoadFile(fopFilename_.c_str());

    tinyxml2::XMLElement* main = fopDoc.FirstChildElement("factorsOfProduction");

    if (main)
    {
        tinyxml2::XMLElement* fop = main->FirstChildElement("product");
        while (fop)
        {
            std::string id = fop->FirstChildElement("id")->GetText();

            std::shared_ptr<CommodityDefinition> product;

            auto& consumerGood = consumerGoodDefs_.find(id);
            if (consumerGood != consumerGoodDefs_.end()) // consumer good found
            {
                product = consumerGood->second;
            }
            else
            {
                auto& commodity = commodityDefs_.find(id);
                if (commodity != commodityDefs_.end()) //commodity found
                {
                    product = commodity->second;
                }
            }

            if (product != nullptr)
            {
                FactorsOfProduction newFop;

                double labour = atof(fop->FirstChildElement("labour")->GetText());
                newFop.setLabout(labour);

                tinyxml2::XMLElement* inputs = fop->FirstChildElement("inputs");
                if (inputs)
                {
                    tinyxml2::XMLElement* input = inputs->FirstChildElement("input");
                    std::string id = input->FirstChildElement("id")->GetText();
                    while (input)
                    {
                        std::string id = input->FirstChildElement("id")->GetText();
                        auto& it = commodityDefs_.find(id);
                        if (it != commodityDefs_.end())
                        {
                            std::shared_ptr<CommodityDefinition> commodity = it->second;
                            double amount = atof(input->FirstChildElement("amount")->GetText());

                            newFop.addInput(commodity, amount);
                        }
                        else
                        {
                            PITCH(Exceptions::OneMessage, "Input commodity not found while reading fops");
                        }

                        input = input->NextSiblingElement();

                    }
                }

                product->setFactorsOfProduction(newFop);
            }
            else
            {
                PITCH(Exceptions::OneMessage, "Product not found while reading fops");
            }

            fop = fop->NextSiblingElement();
        }
    }
    else
    {
        PITCH(Exceptions::OneMessage, "Commodity definitio0ns not found");
    }
}

void DefinitionStorage::readGoodUtils(std::shared_ptr<CommodityDefinition> good, tinyxml2::XMLElement* utilities)
{
    tinyxml2::XMLElement* utility = utilities->FirstChildElement("utility");
    while (utility)
    {
        std::string utilityId = utility->FirstChildElement("id")->GetText();
        double amount = std::atof(utility->FirstChildElement("amount")->GetText());

        auto& actualUtility = utilityDefs_.find(utilityId);
        if (actualUtility == utilityDefs_.end())
        {
            PITCH(Exceptions::OneMessage, "Utility definition not found while reading commodities");
        }

        good->setUtility(actualUtility->second, amount);

        utility = utility->NextSiblingElement();
    }
}

/*void DefinitionStorage::readActivityDefs()
{
    XMLDocument activityDoc;
    activityDoc.LoadFile(activityDefFilename_.c_str());

    XMLElement* main = activityDoc.FirstChildElement("activities");

    if (main)
    {
        XMLElement* activity = main->FirstChildElement("activity");
        while (activity)
        {
            std::string id = activity->FirstChildElement("id")->GetText();
            std::string name = activity->FirstChildElement("name")->GetText();
            std::string description = activity->FirstChildElement("description")->GetText();

            std::shared_ptr<Activity> com = std::make_shared<Activity>(name);
            com->setDescription(description);

            XMLElement* utilities = activity->FirstChildElement("utilities");
            if (utilities)
            {
                XMLElement* utility = utilities->FirstChildElement("utility");
                while (utility)
                {
                    std::string utilityId = utility->FirstChildElement("id")->GetText();
                    double amount = std::atof(utility->FirstChildElement("amount")->GetText());


                    auto& actualUtility = utilityDefs_.find(utilityId)->second;
                    com->addUtility(actualUtility, amount);

                    utility = utility->NextSiblingElement();
                }
            }

            activityDefs_[id] = com;

            activity = activity->NextSiblingElement();
        }
    }
}*/
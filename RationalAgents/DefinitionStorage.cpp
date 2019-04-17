#include "DefinitionStorage.h"
#include "tinyxml2.h"
#include "Utility.h"
#include "Commodity.h"
#include "Exceptions.h"
#include <windows.h>

void DefinitionStorage::readDefinitions()
{
    try
    {
        readUtilityDefs();
        //readActivityDefs(); // depends on utility defs
        readCommodityDefs(); // depends on utility defs
    }
    catch (const Exceptions::OneMessage& e)
    {
        e.print(std::cout);
    }
}

const std::shared_ptr<Commodity>& DefinitionStorage::getCommodity(const std::string& id) const
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

const std::map<std::string, std::shared_ptr<Commodity>>& DefinitionStorage::getCommodityDefinitions() const
{
    return commodityDefs_;
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



            tinyxml2::XMLElement* production = commodity->FirstChildElement("production");
            double labour = 0.0;
            if (production)
            {
                labour = atof(production->FirstChildElement("labour")->GetText());
            }
            FactorsOfProduction productionInput(labour);

            std::shared_ptr<Commodity> com = std::make_shared<Commodity>(name, description, productionInput);

            tinyxml2::XMLElement* utilities = commodity->FirstChildElement("utilities");
            if (utilities)
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



                    com->setUtility(actualUtility->second, amount);

                    utility = utility->NextSiblingElement();
                }
            }

            commodityDefs_[id] = com;

            commodity = commodity->NextSiblingElement();
        }
    }
    else
    {
        PITCH(Exceptions::OneMessage, "Commodity definitio0ns not found");
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
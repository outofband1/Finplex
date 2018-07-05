#include "DefinitionStorage.h"
#include "Utility.h"
#include "Commodity.h"
#include "tinyxml2.h"

using namespace tinyxml2;

void DefinitionStorage::readDefinitions()
{
	readUtilityDefs();
	readCommodityDefs(); // depends on utility defs
}

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
	XMLDocument utilityDoc;
	utilityDoc.LoadFile(utilityDefFilename_.c_str());

	XMLElement* main = utilityDoc.FirstChildElement("utilities");

	XMLElement* utility = main->FirstChildElement("utility");
	while (utility)
	{
		std::string id = utility->FirstChildElement("id")->GetText();
		std::string name = utility->FirstChildElement("name")->GetText();
		std::string description = utility->FirstChildElement("description")->GetText();

		Utility::PieceWiseLinearCurve utilCurve;
		XMLElement* curve = utility->FirstChildElement("curve");
		XMLElement* curvePiece = curve->FirstChildElement("piece");
		while (curvePiece)
		{
			float slope = std::atof(curvePiece->FirstChildElement("slope")->GetText());
			float intercept = std::atof(curvePiece->FirstChildElement("intercept")->GetText());
			utilCurve.addCurvePiece(slope, intercept);

			curvePiece = curvePiece->NextSiblingElement();
		}

		std::shared_ptr<Utility> util = std::make_shared<Utility>(name, utilCurve);
		util->setDescription(description);

		utilityDefs_[id] = util;

		utility = utility->NextSiblingElement();
	}
}

void DefinitionStorage::readCommodityDefs()
{
	XMLDocument commodityDoc;
	commodityDoc.LoadFile(commodityDefFilename_.c_str());

	XMLElement* main = commodityDoc.FirstChildElement("commodities");

	XMLElement* commodity = main->FirstChildElement("commodity");
	while (commodity)
	{
		std::string id = commodity->FirstChildElement("id")->GetText();
		std::string name = commodity->FirstChildElement("name")->GetText();
		std::string description = commodity->FirstChildElement("description")->GetText();

		std::shared_ptr<Commodity> com = std::make_shared<Commodity>(name);
		com->setDescription(description);

		XMLElement* utilities = commodity->FirstChildElement("utilities");
		XMLElement* utility = utilities->FirstChildElement("utility");
		while (utility)
		{
			std::string utilityId = utility->FirstChildElement("id")->GetText();
			float amount = std::atof(utility->FirstChildElement("amount")->GetText());


			auto& actualUtility = utilityDefs_.find(utilityId)->second;
			com->addUtility(actualUtility, amount);

			utility = utility->NextSiblingElement();
		}

		commodityDefs_[id] = com;

		commodity = commodity->NextSiblingElement();
	}

}
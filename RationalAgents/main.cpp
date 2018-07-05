#include "Simplex.h"
#include <tchar.h>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "Utility.h"
#include "Commodity.h"
#include "PurchaseSolver.h"
#include "tinyxml2.h"

using namespace tinyxml2;

void readUtilityDefs(std::map<std::string, std::shared_ptr<Utility>>& utilityDefs)
{
	XMLDocument utilityDoc;
	utilityDoc.LoadFile("def_utility.xml");

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

		utilityDefs[id] = util;

		utility = utility->NextSiblingElement();
	}
}

void readCommodityDefs(std::map<std::string, std::shared_ptr<Commodity>>& commodityDefs, const std::map<std::string, std::shared_ptr<Utility>>& utilityDefs)
{
	XMLDocument commodityDoc;
	commodityDoc.LoadFile("def_commodity.xml");

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
			
			
			auto& actualUtility = utilityDefs.find(utilityId)->second;
			com->addUtility(actualUtility, amount);

			utility = utility->NextSiblingElement();
		}

		commodityDefs[id] = com;

		commodity = commodity->NextSiblingElement();
	}

}

int _tmain(int argc, _TCHAR* argv[])
{
	std::map<std::string, std::shared_ptr<Utility>> utilityDefs;
	std::map<std::string, std::shared_ptr<Commodity>> commodityDefs;

	readUtilityDefs(utilityDefs);
	readCommodityDefs(commodityDefs, utilityDefs);

    float money = 80;

    PurchaseSolver solver;

	int count = 0;
	for (auto& commodity : commodityDefs)
	{
		solver.registerCommodity(commodity.second);
		switch (count)
		{
		case 0:
			solver.setPriceAndAmount(commodity.second, 1.1f, 3.0f);
			break;
		case 1:
			solver.setPriceAndAmount(commodity.second, 1.2f, 4.0f);
			break;
		case 2:
			solver.setPriceAndAmount(commodity.second, 1.3f, -1.0f);
			break;
		}

		count++;
	}
    
	for (auto& utility : utilityDefs)
	{
		solver.registerUtility(utility.second);
	}
    
    // return res
    solver.createSimplexSolver(money);

    std::cout << std::endl;
    system("pause");
    return 0;
}





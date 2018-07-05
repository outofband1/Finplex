#pragma once

#include <map>
#include <memory>

class Utility;
class Commodity;

class DefinitionStorage
{
public:
	void readDefinitions();

	const std::map<std::string, std::shared_ptr<Utility>>& getUtilityDefinitions() const;
	const std::map<std::string, std::shared_ptr<Commodity>>& getCommodityDefinitions() const;
private:
	const std::string utilityDefFilename_ = "def_utility.xml";
	const std::string commodityDefFilename_ = "def_commodity.xml";

	void readUtilityDefs();
	void readCommodityDefs();

	std::map<std::string, std::shared_ptr<Utility>> utilityDefs_;
	std::map<std::string, std::shared_ptr<Commodity>> commodityDefs_;
};
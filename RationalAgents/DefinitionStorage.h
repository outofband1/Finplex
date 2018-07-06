#pragma once

#include <map>
#include <memory>

class Utility;
class Commodity;
class Activity;

class DefinitionStorage
{
public:
	void readDefinitions();

	const std::map<std::string, std::shared_ptr<Activity>>& getActivityDefinitions() const;
	const std::map<std::string, std::shared_ptr<Utility>>& getUtilityDefinitions() const;
	const std::map<std::string, std::shared_ptr<Commodity>>& getCommodityDefinitions() const;
private:
	const std::string activityDefFilename_ = "def_activity.xml";
	const std::string utilityDefFilename_ = "def_utility.xml";
	const std::string commodityDefFilename_ = "def_commodity.xml";

	void readActivityDefs();
	void readUtilityDefs();
	void readCommodityDefs();

	std::map<std::string, std::shared_ptr<Activity>> activityDefs_;
	std::map<std::string, std::shared_ptr<Utility>> utilityDefs_;
	std::map<std::string, std::shared_ptr<Commodity>> commodityDefs_;
};
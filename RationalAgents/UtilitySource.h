#pragma once
#include <string>
#include <map>
#include <memory>

#include "Definition.h"

class Utility;

class UtilitySource : public Definition
{
public:
	UtilitySource(const std::string& name);

	void addUtility(const std::shared_ptr<Utility>& utility, const float& amount);

	const std::map<std::shared_ptr<Utility>, float>& getUtilities() const;

private:
	std::string name_;
	std::string description_;

	std::map<std::shared_ptr<Utility>, float> utilities_;
};
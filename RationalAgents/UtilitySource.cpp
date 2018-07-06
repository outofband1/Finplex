#include "UtilitySource.h"


UtilitySource::UtilitySource(const std::string& name) : Definition(name)
{

}

void UtilitySource::addUtility(const std::shared_ptr<Utility>& utility, const float& amount)
{
	utilities_[utility] = amount;
}

const std::map<std::shared_ptr<Utility>, float>& UtilitySource::getUtilities() const
{
	return utilities_;
}
#include "Inventory.h"

void Inventory::add(const std::shared_ptr<Commodity>& commodity, const float& amount)
{
	auto& currentStore = inventory_.find(commodity);
	if (currentStore != inventory_.end())
	{
		currentStore->second += amount;
	}
	else
	{
		inventory_[commodity] = amount;
	}
}

void Inventory::remove(const std::shared_ptr<Commodity>& commodity, const float& amount)
{
	auto& currentStore = inventory_.find(commodity);
	if (currentStore != inventory_.end() && currentStore->second <= amount)
	{
		currentStore->second -= amount;
	}
	else
	{
		throw;
	}
}

float Inventory::getAmount(const std::shared_ptr<Commodity>& commodity) const
{
	auto& currentStore = inventory_.find(commodity);
	if (currentStore != inventory_.end())
	{
		return currentStore->second;
	}
	else
	{
		return 0.0f;
	}
}

void Inventory::transferTo(Inventory& inventory, const std::shared_ptr<Commodity>& commodity, const float& amount)
{
	remove(commodity, amount);
	inventory.add(commodity, amount);
}
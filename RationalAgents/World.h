#pragma once
#include <vector>
#include <memory>
#include <map>
#include "Facility.h"
#include "Entity.h"
#include "Factory.h"

class Agent;
class Company;

class World
{
public:
    std::shared_ptr<Agent> createAgent(const std::string& name)
    {
        auto& newAgent = std::make_shared<Agent>(name);
        agents_.push_back(newAgent);

        return newAgent;
    }

    std::shared_ptr<Company> createCompany(const std::string& name)
    {
        auto& newCompany = std::make_shared<Company>(name);
        companies_.push_back(newCompany);

        return newCompany;
    }

    std::shared_ptr<Factory> createFactory(const std::shared_ptr<Company>& owner, const std::string& name, const std::shared_ptr<CommodityDefinition>& commodity)
    {
        auto& newFactory = std::make_shared<Factory>(name, commodity);
        newFactory->transferOwnership(owner);
        factories_.push_back(newFactory);

        return newFactory;
    }

    const std::vector<std::shared_ptr<Agent>>& getAgents() const
    {
        return agents_;
    }

    const std::vector<std::shared_ptr<Company>>& getCompanies() const
    {
        return companies_;
    }

    const std::vector<std::shared_ptr<Factory>>& getFactories() const
    {
        return factories_;
    }

private:
    std::vector<std::shared_ptr<Agent>> agents_;
    std::vector<std::shared_ptr<Company>> companies_;
    std::vector<std::shared_ptr<Factory>> factories_;
};
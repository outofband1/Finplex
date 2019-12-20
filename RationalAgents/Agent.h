#pragma once
#include "Entity.h"

class SimulationLocal;

class Agent : public Entity
{
public:
    Agent(const std::string& name);

    const double getBudget() const;

    void submitBudgetForMarketAnalysis(SimulationLocal& sim) const;

    void newRound();

private:

};
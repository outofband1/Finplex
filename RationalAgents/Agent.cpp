#include "Agent.h"
#include "SimulationLocal.h"

Agent::Agent(const std::string& name) :
    Entity(name)
{

}

const double Agent::getBudget() const
{
    return money_;
}

void Agent::submitBudgetForMarketAnalysis(SimulationLocal& sim) const
{
    sim.acceptAgentBudgetForMarketAnalysis(money_);
}

void Agent::newRound()
{

}
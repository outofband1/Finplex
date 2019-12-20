#include <tchar.h>
#include <iostream>
#include <memory>
#include <string>

#include "DefinitionStorage.h"
#include "utility.h"
#include "Factory.h"
#include "MarketSolver.h"
#include <vector>
#include "Agent.h"
#include "ConsumerMarket.h"
#include <stdlib.h>
#include "Inventory.h"
#include "World.h"
#include "SimulationLocal.h"




int _tmain(int argc, _TCHAR* argv[])
{
    DefinitionStorage definitions;
    definitions.readDefinitions();

    World world;

    auto& c1 = world.createCompany("C1");
    auto& c2 = world.createCompany("C2");
    auto& c3 = world.createCompany("C3");
    auto& c4 = world.createCompany("C4");
    auto& c4b = world.createCompany("C4B");
    auto& c5 = world.createCompany("C5");
    auto& c6 = world.createCompany("C6");
    auto& c7 = world.createCompany("C7");
    auto& c8 = world.createCompany("C8");

    std::shared_ptr<Factory> p1 = world.createFactory(c1, "P1", definitions.getConsumerGood("meal"));
    p1->setHourlyWage(10.0);
    p1->setShiftLength(8.0);
    p1->money_ = 1000.0;

    std::shared_ptr<Factory> p2 = world.createFactory(c2, "P2", definitions.getConsumerGood("beer"));
    p2->setHourlyWage(10.0);
    p2->setShiftLength(8.0);
    p2->money_ = 1000.0;

    std::shared_ptr<Factory> p3 = world.createFactory(c3, "P3", definitions.getConsumerGood("luxuries"));
    p3->setHourlyWage(10.0);
    p3->setShiftLength(8.0);
    p3->money_ = 1000.0;

    std::shared_ptr<Factory> p4 = world.createFactory(c4, "P4", definitions.getCommodity("water"));
    p4->setHourlyWage(10.0);
    p4->setShiftLength(8.0);
    p4->money_ = 1000.0;

    std::shared_ptr<Factory> p4b = world.createFactory(c4b, "P4B", definitions.getCommodity("water"));
    p4b->setHourlyWage(10.0);
    p4b->setShiftLength(8.0);
    p4b->money_ = 1000.0;

    std::shared_ptr<Factory> p5 = world.createFactory(c5, "P5", definitions.getCommodity("carbs"));
    p5->setHourlyWage(10.0);
    p5->setShiftLength(8.0);
    p5->money_ = 1000.0;

    std::shared_ptr<Factory> p6 = world.createFactory(c6, "P6", definitions.getCommodity("protein"));
    p6->setHourlyWage(10.0);
    p6->setShiftLength(8.0);
    p6->money_ = 1000.0;

    std::shared_ptr<Factory> p7 = world.createFactory(c7, "P7", definitions.getCommodity("gold"));
    p7->setHourlyWage(10.0);
    p7->setShiftLength(8.0);
    p7->money_ = 1000.0;

    std::shared_ptr<Factory> p8 = world.createFactory(c8, "P8", definitions.getConsumerGood("bar"));
    p8->setHourlyWage(10.0);
    p8->setShiftLength(8.0);
    p8->money_ = 1000.0;

    std::shared_ptr<Agent> bob = world.createAgent("Bob");
    bob->money_ = 0.0;

    SimulationLocal sim;

    std::vector<std::shared_ptr<Agent>> agents_;

    const int beerProducers = 1;
    const int mealProducers = 1;
    const int luxProducers = 1;
    const int waterProducers = 1;
    const int waterProducersB = 1;
    const int carbsProducers = 1;
    const int proteinProducers = 1;
    const int goldProducers = 1;
    const int barProducers = 1;

    int agentCount = 0;

    for (int i = 0; i < mealProducers; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p1->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    for (int i = 0; i < beerProducers; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p2->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    for (int i = 0; i < luxProducers; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p3->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    for (int i = 0; i < waterProducers; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p4->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    for (int i = 0; i < waterProducersB; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p4b->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    for (int i = 0; i < carbsProducers; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p5->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    for (int i = 0; i < proteinProducers; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p6->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    for (int i = 0; i < goldProducers; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p7->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    for (int i = 0; i < barProducers; i++)
    {
        agentCount++;
        std::string newName = "Agent " + std::to_string(agentCount);
        std::shared_ptr<Agent> newAgent = world.createAgent(newName);
        p8->addEmployee(newAgent);
        agents_.push_back(newAgent);
    }

    double taxes = 0.0;
    while (true)
    {
        sim.main(world);

        for (auto& agent : agents_)
        {
            std::cout << agent->getName() << " now has " << agent->money_ << " in the bank" << std::endl;
        }

        std::cout << bob->getName() << " now has " << bob->money_ << " in the bank" << std::endl;

        system("pause");
    }


    system("pause");

    return 0;
}





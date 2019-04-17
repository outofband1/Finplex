#pragma once
#include <string>

class Producer
{
public:
    Producer(const std::string& name);

    const std::string& getName() const;
private:
    std::string name_;

    //int labourCapacity_;

    // owner
};
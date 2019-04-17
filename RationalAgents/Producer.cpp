#include "Producer.h"

Producer::Producer(const std::string& name) :
    name_(name)
{

}

const std::string& Producer::getName() const
{
    return name_;
}
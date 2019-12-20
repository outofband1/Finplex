#pragma once

#include <memory>


class Entity;

class Facility
{
public:
    Facility()
    {
        owner_ = nullptr;
    }

    virtual void transferOwnership(const std::shared_ptr<Company>& newOwner) = 0;

protected:
    void setOwner(const std::shared_ptr<Company>& owner)
    {
        owner_ = owner;
    }

    const std::shared_ptr<Company>& getOwner() const
    {
        return owner_;
    }

private:
    std::shared_ptr<Company> owner_;
};
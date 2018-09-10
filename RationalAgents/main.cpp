#include "Simplex.h"
#include <tchar.h>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "Utility.h"
#include "Commodity.h"
#include "Activity.h"
#include "PurchaseSolver.h"
#include "DefinitionStorage.h"
#include "Inventory.h"
#include "Producer.h"
#include <algorithm>
#include "Entity.h"
#include "Market.h"
#include "TradableGood.h"

#include "lp_lib.h"

class UtilExp
{
public:

    void setCurve(const double& max, const double& saturationAmount, const double& saturationDegree)
    {
        // solve max(1-e^saturationAmount*x)=saturationDegree*max)
        max_ = max;
        coefficient_ = log(-saturationDegree + 1);
        coefficient_ /= saturationAmount;
    }

    double getUtility(const double& amount) const
    {
        double u = max_ * (1.0 - exp(coefficient_ * amount));
        return u;
    }

    double getMarginalUtility(const double& amount) const
    {
        double mu = max_ * -coefficient_ * exp(coefficient_ * amount);
        return mu;
    }

    double getAmountFromMU(const double& targetMU)
    {
        double res = log(-targetMU  / (max_ * coefficient_)) / coefficient_;
        return res;
    }

private:
    double max_;
    double coefficient_;
};

class Basket
{
public:
    void setProductAmount(const std::shared_ptr<UtilExp>& product, const double& amount)
    {
        products[product] = amount;
    }

    double getMarginalUtility(const std::shared_ptr<UtilExp>& product)
    {
        auto& it = products.find(product);

        if (it != products.end())
        {
            return product->getMarginalUtility(it->second);
        }
        else
        {
            return product->getMarginalUtility(0.0);
        }
    }

private:
    std::map <std::shared_ptr<UtilExp>, double> products;

};


class ProductExp
{
public:
    ProductExp(const std::string& name, const bool& isActivity)
    {
        name_ = name;
        isActivity_ = isActivity;
    }

    void setUtility(const std::shared_ptr<UtilExp>& utility, const double& weight)
    {
        utilities[utility] = weight;
    }

    const std::map<std::shared_ptr<UtilExp>, double>& getUtilities() const
    {
        return utilities;
    }

    void setInput(const std::shared_ptr<ProductExp>& product, const double& amount)
    {
        inputProducts_[product] = amount;
    }

    const std::map<std::shared_ptr<ProductExp>, double>& getInputs() const
    {
        return inputProducts_;
    }

    bool isActivity() const
    {
        return isActivity_;
    }

    const std::string& getName() const
    {
        return name_;
    }

private:
    std::map <std::shared_ptr<UtilExp>, double> utilities;
    std::map <std::shared_ptr<ProductExp>, double> inputProducts_;


    std::string name_;

    bool isActivity_;
};

class OptimizerExp
{
public:
    void setSleepUtility(const std::shared_ptr<UtilExp>& sleepUtil)
    {
        sleepUtil_ = sleepUtil;
    }

    void addProduct(const std::shared_ptr<ProductExp>& product, const double& capacity, const double& minPrice)
    {
        PriceAndAmount newPanda;
        newPanda.amount_ = capacity;
        newPanda.minPrice_ = minPrice;
        newPanda.price_ = 0.0;
        newPanda.mu_ = 0.0;
        newPanda.inputCost_ = 0.0;
        newPanda.reservedAmount_ = 0.0;

        sleepAmount_ = 0.0;

        pandas_[product] = newPanda;
    }

    void optimize(const double& hours)
    {
        sleepAmount_ = hours;

        refreshUtilities();

        double balance = -1.0;

        // initialize sub resources amounts (circular dependencies must be avoided?)
        refreshReservedAmounts();

        // initialize minimum prices from time and price of sub resources

        double sleepMu = sleepUtil_->getMarginalUtility(sleepAmount_);
        do
        {
            double spend = 0.0;
            bool updateSleep = true;
            for (auto& panda : pandas_)
            {
                if (panda.first->isActivity())
                {
                    panda.second.price_ = (panda.second.mu_ - sleepMu) / sleepMu;
                    spend += 1.0 * panda.second.amount_;
                }
                else
                {
                    panda.second.price_ = panda.second.mu_ / sleepMu;
                }

                if (panda.second.price_ < panda.second.minPrice_ + panda.second.inputCost_ - 10E-6)
                {
                    panda.second.amount_ *= 0.999;
                    // update sub resources amounts
                    // this lowers price of sub-product and thereby minimum price of primary product
                    refreshReservedAmounts();

                    // a lower minimum price of primary product increases demand of primary product. Will it balance?
                    updateSleep = false;
                }
            }

            if (!updateSleep)
            {
                refreshUtilities();

                for (auto& panda : pandas_)
                {
                    if (panda.first->isActivity())
                    {
                        panda.second.price_ = (panda.second.mu_ - sleepMu) / sleepMu;
                        //spend += 1.0 * panda.second.amount_;
                    }
                    else
                    {
                        panda.second.price_ = panda.second.mu_ / sleepMu;
                    }
                }

                // recalculate minimum prices from time and price of sub resources
                refreshReservedAmounts();
            }
            else
            {
                for (auto& panda : pandas_)
                {
                    spend += panda.second.price_ * (panda.second.amount_ - panda.second.reservedAmount_);
                }

                balance = hours - sleepAmount_ - spend;
                sleepAmount_ *= 0.999;
                sleepMu = sleepUtil_->getMarginalUtility(sleepAmount_);
            }
        }
        while (balance < 0);


        int uhuh = 0;
    }

    void print()
    {
        double totalSpend = 0.0;

        for (auto& panda : pandas_)
        {
            std::cout << panda.first->getName() << ":" << std::endl;
            std::cout << panda.second.amount_ - panda.second.reservedAmount_ << " at price: " << panda.second.price_ << std::endl;
            std::cout << panda.second.mu_ / panda.second.price_ << std::endl << std::endl;

            totalSpend += (panda.second.amount_ - panda.second.reservedAmount_) * panda.second.price_;
        }

        std::cout << "Total spend: " << totalSpend << std::endl << std::endl;

        double sleepMu = sleepUtil_->getMarginalUtility(sleepAmount_);
        std::cout << "while sleeping for: " << sleepAmount_ << " hours. at MU: " << sleepMu << std::endl;
    }

private:
    struct PriceAndAmount
    {
        double minPrice_;
        double price_;
        double amount_;
        double reservedAmount_;
        double inputCost_;
        double mu_;
    };

    void refreshInputCosts()
    {

    }

    void refreshReservedAmounts()
    {
        for (auto& panda : pandas_)
        {
            panda.second.reservedAmount_ = 0.0;
            for (auto& input : panda.first->getInputs())
            {
                double reservedAmount = input.second * panda.second.amount_;
                auto& inputProduct = pandas_.find(input.first);
                inputProduct->second.reservedAmount_ += reservedAmount;
            }
        }

        for (auto& panda : pandas_)
        {
            panda.second.inputCost_ = 0.0;
            for (auto& input : panda.first->getInputs())
            {
                double reservedAmount = input.second * panda.second.amount_;
                auto& inputProduct = pandas_.find(input.first);
                panda.second.inputCost_ += inputProduct->second.price_;
            }

        }


    }

    void refreshUtilities()
    {
        calculateUtilityAmounts();

        for (auto& panda2 : pandas_)
        {
            panda2.second.mu_ = getMarginalUtility(panda2.first);
        }
    }

    void calculateUtilityAmounts()
    {
        utilityAmounts_.clear();
        for (auto& panda : pandas_)
        {
            double amount = panda.second.amount_ - panda.second.reservedAmount_;
            for (auto& util : panda.first->getUtilities())
            {
                double weight = util.second;
                auto& utilityAmount = utilityAmounts_.find(util.first);
                if (utilityAmount != utilityAmounts_.end())
                {
                    utilityAmount->second += weight * amount;
                }
                else
                {
                    utilityAmounts_[util.first] = weight * amount;
                }

            }
        }
    }

    double getMarginalUtility(const std::shared_ptr<ProductExp>& product) const
    {
        double mu = 0.0;
        for (const auto& util : product->getUtilities())
        {
            auto& it = utilityAmounts_.find(util.first);
            double utilityAmount = it->second;
            mu += util.second * util.first->getMarginalUtility(utilityAmount);
        }

        return mu;
    }

    std::shared_ptr<UtilExp> sleepUtil_;

    std::map<std::shared_ptr<ProductExp>, PriceAndAmount> pandas_;

    std::map <std::shared_ptr<UtilExp>, double> utilityAmounts_;

    double sleepAmount_;
};

int _tmain(int argc, _TCHAR* argv[])
{
    std::shared_ptr<UtilExp> uFood = std::make_shared<UtilExp>();
    uFood->setCurve(10, 3, 0.95);

    std::shared_ptr<UtilExp> uDrink = std::make_shared<UtilExp>();
    uDrink->setCurve(10, 3, 0.95);

    std::shared_ptr<UtilExp> uSleep = std::make_shared<UtilExp>();
    uSleep->setCurve(10, 8, 0.95);

    std::shared_ptr<UtilExp> uRelax = std::make_shared<UtilExp>();
    uRelax->setCurve(1, 2, 0.95);

    std::shared_ptr<ProductExp> meal1 = std::make_shared<ProductExp>("Meal 1", false);
    meal1->setUtility(uFood, 1.0);

    std::shared_ptr<ProductExp> meal2 = std::make_shared<ProductExp>("Meal 2", false);
    meal2->setUtility(uFood, 1.0);

    std::shared_ptr<ProductExp> beer1 = std::make_shared<ProductExp>("Beer 1", false);
    beer1->setUtility(uFood, 0.1);
    beer1->setUtility(uDrink, 0.9);

    std::shared_ptr<ProductExp> relax1 = std::make_shared<ProductExp>("Relax 1", true);
    relax1->setUtility(uRelax, 1.0);
    relax1->setInput(beer1, 1.0);

    OptimizerExp opt;
    opt.setSleepUtility(uSleep);
    opt.addProduct(meal1, 3, 2.0);
    opt.addProduct(meal2, 2, 2.0);
    opt.addProduct(beer1, 4, 2.0);
    opt.addProduct(relax1, 2, 2.0);

    opt.optimize(24.0);

    opt.print();

    /*UtilExp food;
    food.setCurve(6, 3, 0.95);

    UtilExp drink;
    drink.setCurve(10, 3, 0.95);

    UtilExp sleep;
    sleep.setCurve(6, 8, 0.95);

    UtilExp relax;
    relax.setCurve(6, 3, 0.95);


    double s = 24;
    double balance = -1;

    double dAmount = 4;

    while (true)
    {
        double foodMu = food.getMarginalUtility(3);
        double drinkMu = drink.getMarginalUtility(4);
        double relaxMu = relax.getMarginalUtility(3);
        double sleepMu = sleep.getMarginalUtility(s);

        double sleepPrice = 1;
        double relaxPrice = (relaxMu - sleepMu) / sleepMu;
        double foodPrice = foodMu / sleepMu;
        double drinkPrice = drinkMu / sleepMu;

        //         if (drinkPrice < 2.0 - 10E-6)
        //         {
        //             dAmount = drink.getAmountFromMU(2.0 * sleepMu);
        //         }
        //         else
        //         {
        balance = (24 - s - 3) - relaxPrice - 3 * foodPrice - 4 * drinkPrice;

        double a = foodMu / foodPrice;
        double b = drinkMu / drinkPrice;
        double c = sleepMu / sleepPrice;
        double d = relaxMu / relaxPrice;


        s -= 0.01;

        //        }
    }*/

    system("pause");

    return 0;
}





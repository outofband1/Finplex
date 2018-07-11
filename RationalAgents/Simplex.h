#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <set>

template <typename TYPE>
class SimplexSolver
{
public:

    SimplexSolver(const size_t& rows, const size_t& cols) :
        rowCount_(rows),
        colCount_(cols)
    {
        entries_.resize(rowCount_ * colCount_);
    }

    void setTableau(const std::vector<TYPE>& values)
    {
        entries_ = values;
    }

    inline
    const TYPE& getEntry(const size_t& row, const size_t& col) const
    {
        return entries_[row * colCount_ + col];
    }

    inline
    void setEntry(const size_t& row, const size_t& col, const TYPE& val)
    {
        entries_[row * colCount_ + col] = val;
    }

    void printBasicVariables()
    {
        std::cout.precision(4);
        std::cout << std::fixed << std::showpos;
        std::cout << std::endl;

        if (!initialized_)
        {
            initialize();
        }

        for (size_t basicIndex = 0; basicIndex < basic_.size(); basicIndex++)
        {
            TYPE val = getEntry(basic_[basicIndex].row, colCount_ - 1);
            std::cout << "Basic var " << basic_[basicIndex].col + 1 << ": " << val << std::endl;
        }
    }

    void printTableau()
    {
        std::cout.precision(2);
        std::cout << std::fixed << std::showpos;
        std::cout << std::endl;

        for (double j = 0; j < colCount_; j++) // column
        {
            std::cout << j / 100 << ", ";
        }
        std::cout << std::endl;
        for (size_t i = 0; i < rowCount_; i++) // row
        {
            for (size_t j = 0; j < colCount_; j++) // column
            {
                std::cout << getEntry(i, j);
                if (j < colCount_ - 1)
                {
                    std::cout << ", ";
                }
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void Solve(TYPE& max, std::vector<TYPE>& variables)
    {
        basic_.reserve(rowCount_);
        nonbasic_.reserve(rowCount_);
        bool done = false;
        while (!done)
        {
            // find entering variable / pivot column
            size_t enteringColumnIndex = findEnteringVariableIndex();

            if (enteringColumnIndex == -1) // all coefficient positive -> solution found;
            {
                done = true;
                continue;
            }

            // find pivot row
            size_t pivotRow = findPivotRowIndex(enteringColumnIndex);

            if (pivotRow == -1)
            {
                printTableau();
                std::cout << "Problem unbounded" << std::endl;
                return; // all coefficients negative -> problem unbounded
            }

            pivot(pivotRow, enteringColumnIndex);
        }
        findBasicVariables();


        size_t basc = basic_.size();
        variables.reserve(basic_.size() + nonbasic_.size());
        for (size_t i = 0; i < basic_.size() + nonbasic_.size(); i++)
        {
            variables.push_back(0);
        }

        bool alt = true;
        std::set<size_t> alreadyPivoted;
        /*for (Variabel& var : basic_)
        {
            alreadyPivoted.insert(var.col);
        }*/

        for (size_t basicIndex = 0; basicIndex < basic_.size(); basicIndex++)
        {
            TYPE val = getEntry(basic_[basicIndex].row, colCount_ - 1);
            variables[basic_[basicIndex].col] += val;
        }


        /*int altSolutionsFound = 0;
        while (alt)
        {
            alt = false;
            for (const Variabel& v : nonbasic_)
            {
                if (alreadyPivoted.find(v.col) == alreadyPivoted.end() && v.row != -1 && abs(getEntry(0, v.col)) == 0)// 10E-6???!? // if an alternate solution exist we will have a nonbasic var with cost = 0
                {
                    size_t pivotRow = findPivotRowIndex(v.col);
                    pivot(pivotRow, v.col);
                    alreadyPivoted.insert(v.col);
                    alt = true;
                    break;
                }
            }

            if (alt)
            {
                printTableau();
                findBasicVariables();

                altSolutionsFound++;
                for (size_t basicIndex = 0; basicIndex < basic_.size(); basicIndex++)
                {
                    TYPE val = getEntry(basic_[basicIndex].row, colCount_ - 1);
                    variables[basic_[basicIndex].col] += val;
                }
            }

        }

        max = getEntry(0, colCount_ - 1);

        for (size_t var = 0; var < variables.size(); var++)
        {
            variables[var] /= altSolutionsFound + 1;
        }*/



    }

private:
    struct Variabel
    {
        size_t row, col;
    };

    inline
    void multiplyRow(const size_t& row, const TYPE& multiplier)
    {
        for (size_t i = row * colCount_; i < row * colCount_ + colCount_; i++)
        {
            entries_[i] *= multiplier;
        }
    }

    inline
    size_t findEnteringVariableIndex() const
    {
        TYPE minValue = -10E-6;
        size_t minPos = -1;
        for (size_t i = 0; i < colCount_; i++)
        {
            TYPE entry = getEntry(0, i);
            if (entry < minValue)
            {
                minValue = entry;
                minPos = i;
            }
        }

        return minPos;
    }

    inline
    size_t findPivotRowIndex(const size_t& enteringColumnIndex) const
    {
        size_t pivotRow = -1;
        TYPE smallestRatio = std::numeric_limits<TYPE>::max();
        for (size_t i = 1; i < rowCount_; i++)
        {
            TYPE evc = getEntry(i, enteringColumnIndex); // entering variable coefficient
            if (evc <= 0)
            {
                continue;
            }
            TYPE rhs = getEntry(i, colCount_ - 1); // right hand side

            TYPE newRatio = rhs / evc;

            if (newRatio < smallestRatio)
            {
                smallestRatio = newRatio;
                pivotRow = i;
            }
        }

        return pivotRow;
    }

    inline
    void pivot(const size_t& row, const size_t& col)
    {
        const TYPE& multValue = 1 / getEntry(row, col);
        multiplyRow(row, multValue);

        for (size_t i = 0; i < rowCount_; i++)
        {
            if (i != row && getEntry(i, col) != 0)
            {
                TYPE pivotMult = getEntry(i, col);
                for (size_t j = 0; j < colCount_; j++)
                {
                    TYPE newValue = getEntry(i, j) - pivotMult * getEntry(row, j);
                    setEntry(i, j, newValue);
                }
            }

        }
    }

    void findBasicVariables()
    {
        basic_.clear();
        nonbasic_.clear();
        for (size_t col = 0; col < colCount_ - 1; col++)
        {
            int oneFound = 0;
            int otherFound = 0;
            size_t pivotRow = -1;
            for (size_t row = 0; row < rowCount_; row++)
            {
                const TYPE& val = getEntry(row, col);
                if (abs(val - 1) < 10E-6)
                {
                    oneFound++;
                    pivotRow = row;
                }
                else if (abs(val) > 10E-6)
                {
                    otherFound++;
                }
            }

            if (otherFound == 0 && oneFound == 1)
            {
                Variabel b;
                b.col = col;
                b.row = pivotRow;
                basic_.push_back(b);
            }
            else
            {
                Variabel n;
                n.col = col;
                n.row = pivotRow;
                nonbasic_.push_back(n);
            }

        }

        /*if (basic_.size() < rowCount_ - 1) // start at 1 and cost row
        {
            std::cout << std::endl << "Problem infeasable." << std::endl << std::endl;
            printTableau();
        }*/
    }

    inline
    void initialize()
    {
        findBasicVariables();
    }

    const size_t rowCount_;
    const size_t colCount_;

    std::vector<TYPE> entries_;
    std::vector<Variabel> basic_;
    std::vector<Variabel> nonbasic_;

    bool initialized_ = false;


};
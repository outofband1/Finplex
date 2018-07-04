#pragma once
#include <iostream>


/*
TODO:
unhandled when multiple optimal solutions. f.x
SimplexTableau<float, 3, 5> m;

float data[] =
{
-1, -0.5, +0, +0, +0,
+2, +1, +1, +0, +4,
+1, +2, +0, +1, +3,
};
*/

template <typename TYPE, int ROWS, int COLUMNS>
class SimplexTableau
{
public:
    void setTableau(const TYPE values[COLUMNS * ROWS])
    {
        memcpy(entries_, values, ROWS * COLUMNS * sizeof(TYPE));
    }

    inline
    const TYPE& getEntry(const size_t& row, const size_t& col) const
    {
        return entries_[row * COLUMNS + col];
    }

    inline
    void setEntry(const size_t& row, const size_t& col, const TYPE& val)
    {
        entries_[row * COLUMNS + col] = val;
    }

    void print()
    {
        if (!initialized_)
        {
            initialize();
        }

        std::cout.precision(2);
        std::cout << std::fixed << std::showpos;
        std::cout << std::endl;

        for (size_t basicIndex = 0; basicIndex < basicCount_; basicIndex++)
        {
            TYPE val = getEntry(basic_[basicIndex].row, COLUMNS - 1);
            std::cout << "Basic var " << basic_[basicIndex].col + 1 << ": " << val << std::endl;
        }

        for (size_t i = 0; i < ROWS; i++) // row
        {
            for (size_t j = 0; j < COLUMNS; j++) // column
            {
                std::cout << getEntry(i, j);
                if (j < COLUMNS - 1)
                {
                    std::cout << ", ";
                }
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void Solve(TYPE& max, TYPE variables[ROWS - 1])
    {
        bool done = false;
        while (!done)
        {
            findBasicVariables();

            // find entering variable / pivot column
            size_t enteringColumnIndex = findEnteringVariableIndex();

            if (enteringColumnIndex == -1) // all coefficient positive -> solution found;
            {
                done = true;
                continue;
            }

            // find pivot row
            int pivotRow = findPivotRowIndex(enteringColumnIndex);

            if (pivotRow == -1)
            {
                std::cout << "Problem unbounded" << std::endl;
                return; // all coefficients negative -> problem unbounded
            }

            pivot(pivotRow, enteringColumnIndex);
        }

        //print();

        findBasicVariables();

        /*for (size_t i = 0; i < basicCount_; i++)
        {
        if (getEntry(0, basic_[i].col) != 0)
        {

        }
        }*/


        max = getEntry(0, COLUMNS - 1);



        for (size_t basicIndex = 0; basicIndex < basicCount_; basicIndex++)
        {
            TYPE val = getEntry(basic_[basicIndex].row, COLUMNS - 1);
            variables[basicIndex] = val;
        }
    }

private:
    struct BasicVariabel
    {
        size_t row, col;
    };

    inline
    void multiplyRow(const size_t& row, const TYPE& multiplier)
    {
        for (size_t i = row * COLUMNS; i < row * COLUMNS + COLUMNS; i++)
        {
            entries_[i] *= multiplier;
        }
    }

    inline
    size_t findEnteringVariableIndex() const
    {
        for (size_t i = 0; i < COLUMNS; i++)
        {
            if (getEntry(0, i) < 0)
            {
                return i;
            }
        }

        return -1;
    }

    inline
    size_t findPivotRowIndex(const size_t& enteringColumnIndex) const
    {
        int pivotRow = -1;
        TYPE smallestRatio = std::numeric_limits<TYPE>::max();
        for (size_t i = 1; i < ROWS; i++)
        {
            TYPE evc = getEntry(i, enteringColumnIndex); // entering variable coefficient
            if (evc <= 0)
            {
                continue;
            }
            TYPE rhs = getEntry(i, COLUMNS - 1); // right hand side

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

        for (size_t i = 0; i < ROWS; i++)
        {
            if (i != row && getEntry(i, col) != 0)
            {
                TYPE pivotMult = getEntry(i, col);
                for (size_t j = 0; j < COLUMNS; j++)
                {
                    TYPE newValue = getEntry(i, j) - pivotMult * getEntry(row, j);
                    setEntry(i, j, newValue);
                }
            }

        }
    }

    void findBasicVariables()
    {
        size_t basicIndex = 0;
        size_t nonbasicIndex = 0;
        for (size_t col = 0; col < COLUMNS - 1; col++)
        {
            int oneFound = 0;
            int otherFound = 0;
            size_t pivotRow = -1;
            for (size_t row = 1; row < ROWS; row++)
            {
                const TYPE& val = getEntry(row, col);
                if (val == 1)
                {
                    oneFound++;
                    pivotRow = row;
                }
                else if (val != 0)
                {
                    otherFound++;
                    continue;
                }
            }

            if (otherFound == 0 && oneFound == 1)
            {
                basic_[basicIndex].col = col;
                basic_[basicIndex].row = pivotRow;
                basicIndex++;
            }
            else
            {
                nonBasicColumn_[nonbasicIndex] = col;
                nonbasicIndex++;
            }
        }

        if (basicIndex + 1 < ROWS - 1)
        {
            std::cout << "Problem infeasable." << std::endl;
        }
    }

    inline
    void initialize()
    {
        findBasicVariables();
    }

    const size_t nonBasicCount_ = 4;
    const size_t basicCount_ = 5;

    TYPE entries_[COLUMNS * ROWS];
    BasicVariabel basic_[5];
    size_t nonBasicColumn_[4];
    bool initialized_ = false;


};
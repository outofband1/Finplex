
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <ostream>
#include <memory>
#include <string>
#include <map>
#include <vector>


// M columns, N rows
template <typename TYPE, int ROWS, int COLUMNS>
class SimplexTableau
{
public:
	void setTableau(const TYPE values[COLUMNS * ROWS])
	{
		memcpy(entries, values, ROWS * COLUMNS * sizeof(TYPE));
	}

	inline
		const TYPE& getEntry(const size_t& row, const size_t& col)
	{
			return entries[row * COLUMNS + col];
		}

	inline
		void setEntry(const size_t& row, const size_t& col, const TYPE& val)
	{
			entries[row * COLUMNS + col] = val;
	}

	void print()
	{
		std::cout.precision(2);
		std::cout << std::fixed << std::showpos;
		std::cout << std::endl;
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

	void Solve()
	{
		bool done = false;
		while (!done)
		{
			print();
			// find entering variable / pivot column
			size_t enteringColumnIndex = -1;
			for (size_t i = 0; i < COLUMNS; i++)
			{
				if (getEntry(0, i) < 0)
				{
					enteringColumnIndex = i;
					break;
				}
			}

			if (enteringColumnIndex == -1)
			{
				done = true;
				continue;
			}

			// find pivot row
			int pivotRow = -1;
			TYPE smallestRatio = std::numeric_limits<TYPE>::max();
			for (size_t i = 1; i < ROWS; i++)
			{
				TYPE evc = getEntry(i, enteringColumnIndex); // entering variable coefficient
				if (evc <= 0)
					continue;
				TYPE rhs = getEntry(i, COLUMNS-1); // right hand side
				
				TYPE newRatio = rhs / evc;
				
				if (newRatio < smallestRatio)
				{
					smallestRatio = newRatio;
					pivotRow = i;
				}
			}

			if (pivotRow == -1)
			{
				std::cout << "Problem unbounded" << std::endl;
				return; // all coefficients negative -> problem unbounded
			}
				

			pivot(pivotRow, enteringColumnIndex);
		}
	}

private:
	inline
		void multiplyRow(const size_t& row, const TYPE& multiplier)
	{
		for (size_t i = row * COLUMNS; i < row * COLUMNS + COLUMNS; i++)
		{
			entries[i] *= multiplier;
		}
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

	TYPE entries[COLUMNS * ROWS];
};

int _tmain(int argc, _TCHAR* argv[])
{
	SimplexTableau<float, 3, 5> m;

	float data[] =
	{
		-2, -1, +0, +0, +0,
		-1, +1, +1, +0, +1,
		+1, -2, +0, +1, +2
	};

	m.setTableau(data);

	m.Solve();

	


	std::cout << std::endl;
	system("pause");
	return 0;
}    





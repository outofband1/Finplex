
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
	void setRow(const int& row, const TYPE values[COLUMNS])
	{
		memcpy(&entries[row * COLUMNS], values, COLUMNS * sizeof(TYPE));
	}

	void setTableau(const TYPE values[COLUMNS * ROWS])
	{
		memcpy(entries, values, ROWS * COLUMNS * sizeof(TYPE));
	}

	inline
	const TYPE& getEntry(const int& row, const int& col)
	{
			return entries[row * COLUMNS + col];
		}

	inline
	void setEntry(const int& row, const int& col, const TYPE& val)
	{
			entries[row * COLUMNS + col] = val;
		}

	inline
	void pivot(const int& row, const int& col)
	{
			const TYPE& multValue = 1 / getEntry(row, col);
			multiplyRow(row, multValue);

			for (int i = 0; i < ROWS; i++)
			{
				if (i != row && getEntry(i, col) != 0)
				{
					TYPE pivotMult = getEntry(i, col);
					for (int j = 0; j < COLUMNS; j++)
					{
						TYPE newValue = getEntry(i, j) - pivotMult * getEntry(row, j);
						setEntry(i, j, newValue);
					}
				}

			}
		}

	void print()
	{
		std::cout.precision(2);
		std::cout << std::fixed << std::showpos;
		std::cout << std::endl;
		for (int i = 0; i < ROWS; i++) // row
		{
			for (int j = 0; j < COLUMNS; j++) // column
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
private:
	inline
		void multiplyRow(const int& row, const TYPE& multiplier)
	{
			for (int i = row * COLUMNS; i < row * COLUMNS + COLUMNS; i++)
			{
				entries[i] *= multiplier;
			}
		}

	TYPE entries[COLUMNS * ROWS];
};

int _tmain(int argc, _TCHAR* argv[])
{
	SimplexTableau<float, 4, 6> m;

	float data[] =
	{
		-2, -1, +0, +0, +0, +0,
		+3, +1, +1, +0, +0, +6,
		+1, -1, +0, +1, +0, +2,
		+0, +1, +0, +0, +1, +3
	};

	m.setTableau(data);

	m.print();

	m.pivot(2, 0);

	m.print();

	m.pivot(1, 1);

	m.print();

	m.pivot(3, 3);

	m.print();


	std::cout << std::endl;
	system("pause");
	return 0;
}    





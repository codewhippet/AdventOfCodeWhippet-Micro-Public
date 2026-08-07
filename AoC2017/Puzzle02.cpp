#include "stdafx.h"

using namespace std;

namespace Puzzle02_2017_Types
{
}

using namespace Puzzle02_2017_Types;

void Puzzle02_A_2017()
{
	int32_t answer = 0;

	char line[128];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);

		vector<int32_t> row;
		row.reserve(32);
		Parse::ReadAsVectorOfNumbers(line, &row);

		ranges::sort(row);
		answer += row.back() - row.front();
	}

	PuzzleOutput::Submit(2017, 2, 1, answer);
}

void Puzzle02_B_2017()
{
	int32_t answer = 0;

	char line[128];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);

		vector<int32_t> row;
		row.reserve(32);
		Parse::ReadAsVectorOfNumbers(line, &row);

		for (const auto& p : AllUnorderedPairs(row.size()))
		{
			int32_t a = row[p.first];
			int32_t b = row[p.second];
			assert(a != 0);
			assert(b != 0);

			if (a > b)
			{
				int32_t aDivB = a / b;
				if ((aDivB * b) == a)
				{
					answer += aDivB;
				}
			}
			else if ((b > a) && ((b % a) == 0))
			{
				int32_t bDivA = b / a;
				if ((bDivA * a) == b)
				{
					answer += bDivA;
				}
			}
		}
	}

	PuzzleOutput::Submit(2017, 2, 2, answer);
}

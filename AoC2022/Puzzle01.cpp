#include "stdafx.h"

using namespace std;

namespace Puzzle01_2022_Types
{
}

using namespace Puzzle01_2022_Types;

void Puzzle01_A_2022()
{
	int32_t answer = 0;

	int32_t runningTotal = 0;
	while (PuzzleInput::PeekChar() != EOF)
	{
		if (PuzzleInput::IsEmptyLine())
		{
			answer = max(runningTotal, answer);
			runningTotal = 0;
		}
		else
		{
			runningTotal += Parse::GetInt32();
		}

		Parse::DiscardExpected("\n");
	}

	answer = max(runningTotal, answer);

	PuzzleOutput::Submit(2022, 1, 1, answer);
}

void Puzzle01_B_2022()
{
	const size_t expectedNumElves = 256;
	vector<int32_t> elfTotals;
	elfTotals.reserve(expectedNumElves);

	int32_t runningTotal = 0;
	while (PuzzleInput::PeekChar() != EOF)
	{
		if (PuzzleInput::IsEmptyLine())
		{
			elfTotals.push_back(runningTotal);
			runningTotal = 0;
		}
		else
		{
			runningTotal += Parse::GetInt32();
		}

		Parse::DiscardExpected("\n");
	}

	elfTotals.push_back(runningTotal);
	ranges::sort(elfTotals);

	int32_t answer = accumulate(elfTotals.end() - 3, elfTotals.end(), 0);

	PuzzleOutput::Submit(2022, 1, 2, answer);
}

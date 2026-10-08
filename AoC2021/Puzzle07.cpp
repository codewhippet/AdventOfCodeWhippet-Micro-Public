#include "stdafx.h"

using namespace std;

namespace Puzzle07_2021_Types
{
}

using namespace Puzzle07_2021_Types;

void Puzzle07_A_2021()
{
	const size_t crabCountsReserveSize = 2000;

	vector<int32_t> crabCounts(crabCountsReserveSize);

	MaxValue<int32_t> maxPosition;
	while (PuzzleInput::PeekChar() != '\n')
	{
		int32_t crabPosition = Parse::GetInt32();
		maxPosition.Update(crabPosition);
		crabCounts[crabPosition]++;
	}

	crabCounts.resize(maxPosition + 1);

	vector<int32_t> increaseBy(maxPosition + 1);
	{
		increaseBy[0] = crabCounts[0];
		for (size_t pos = 1; pos < crabCounts.size(); pos++)
		{
			increaseBy[pos] = increaseBy[pos - 1] + crabCounts[pos];
		}
	}

	vector<int32_t> reduceBy(maxPosition + 1);
	{
		reduceBy.back() = 0;
		for (size_t pos = crabCounts.size() - 1; pos > 0; pos--)
		{
			reduceBy[pos - 1] = reduceBy[pos] + crabCounts[pos];
		}
	}

	MinValue<pair<int32_t, int32_t>> targetPosition({ numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() });

	vector<int32_t> fuelUsed(maxPosition + 1);
	{
		int32_t fuel = 0;
		for (int32_t pos = 0; pos < static_cast<int32_t>(crabCounts.size()); pos++)
		{
			fuelUsed[pos] = fuel;
			targetPosition.Update({ fuel, pos });
			fuel = fuel + increaseBy[pos] - reduceBy[pos];
		}
	}

	int32_t answer = 0;
	for (int32_t i = 0; i < static_cast<int32_t>(crabCounts.size()); i++)
	{
		answer += abs(i - targetPosition.Get().second) * crabCounts[i];
	}

	PuzzleOutput::Submit(2021, 7, 1, answer);
}

void Puzzle07_B_2021()
{
	const size_t crabCountsReserveSize = 2000;

	vector<int32_t> crabCounts(crabCountsReserveSize);

	MaxValue<int32_t> maxPosition;
	while (PuzzleInput::PeekChar() != '\n')
	{
		int32_t crabPosition = Parse::GetInt32();
		maxPosition.Update(crabPosition);
		crabCounts[crabPosition]++;
	}

	crabCounts.resize(maxPosition + 1);

	vector<int32_t> fuelUsed(maxPosition + 1);
	{
		for (int32_t crabPos = 0; crabPos < static_cast<int32_t>(crabCounts.size()); crabPos++)
		{
			for (int32_t fuelUsedPos = 0; fuelUsedPos < static_cast<int32_t>(fuelUsed.size()); fuelUsedPos++)
			{
				int32_t diff = abs(crabPos - fuelUsedPos);
				int32_t triangle = (diff * (diff + 1)) / 2;
				fuelUsed[fuelUsedPos] += triangle * crabCounts[crabPos];
			}
		}
	}

	int32_t answer = ranges::min(fuelUsed);

	PuzzleOutput::Submit(2021, 7, 2, answer);
}

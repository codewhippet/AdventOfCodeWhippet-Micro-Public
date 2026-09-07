#include "stdafx.h"

using namespace std;

namespace Puzzle01_2020_Types
{
}

using namespace Puzzle01_2020_Types;

void Puzzle01_A_2020()
{
	HashSet<int32_t> numbers(512, -1);
	while (PuzzleInput::NextLine())
	{
		numbers.Insert(Parse::GetInt32());
	}

	int32_t answer = 0;
	for (int32_t a : numbers)
	{
		int32_t opposite = 2020 - a;
		if (numbers.Contains(opposite))
		{
			answer = a * opposite;
			break;
		}
	}

	PuzzleOutput::Submit(2020, 1, 1, answer);
}

void Puzzle01_B_2020()
{
	vector<int32_t> numbers;
	numbers.reserve(256);

	HashSet<int32_t> numberSet(512, -1);
	while (PuzzleInput::NextLine())
	{
		int32_t number = Parse::GetInt32();
		numbers.push_back(number);
		numberSet.Insert(number);
	}

	int32_t answer = 0;
	for (const auto& p : AllUnorderedPairs(numbers.size()))
	{
		int32_t remaining = 2020 - numbers[p.first] - numbers[p.second];
		if (numberSet.Contains(remaining))
		{
			answer = numbers[p.first] * numbers[p.second] * remaining;
			break;
		}
	}

	PuzzleOutput::Submit(2020, 1, 2, answer);
}

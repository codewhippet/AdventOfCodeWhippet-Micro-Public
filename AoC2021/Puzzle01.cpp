#include "stdafx.h"

using namespace std;

namespace Puzzle01_2021_Types
{
}

using namespace Puzzle01_2021_Types;

void Puzzle01_A_2021()
{
	int32_t numIncreases = 0;

	int32_t currentDepth = Parse::GetInt32();
	while (PuzzleInput::NextLine())
	{
		int32_t nextDepth = Parse::GetInt32();
		if (nextDepth > currentDepth)
		{
			numIncreases++;
		}
		currentDepth = nextDepth;
	}

	int32_t answer = numIncreases;
	PuzzleOutput::Submit(2021, 1, 1, answer);
}

void Puzzle01_B_2021()
{
	const int32_t windowSize = 4;
	const int32_t windowMask = windowSize - 1;

	int32_t numIncreases = 0;

	array<int32_t, windowSize> window;
	for (size_t i = 0; i < window.size() - 1; i++)
	{
		window[i] = Parse::GetInt32();
	}

	int32_t current = static_cast<int32_t>(window.size() - 1);
	while (PuzzleInput::NextLine())
	{
		window[current & windowMask] = Parse::GetInt32();

		int32_t currentDepth = window[(current - 3) & windowMask] + window[(current - 2) & windowMask] + window[(current - 1) & windowMask];
		int32_t nextDepth = window[(current - 2) & windowMask] + window[(current - 1) & windowMask] + window[(current  - 0) & windowMask];
		if (nextDepth > currentDepth)
		{
			numIncreases++;
		}

		current++;
	}

	int32_t answer = numIncreases;
	PuzzleOutput::Submit(2021, 1, 2, answer);
}

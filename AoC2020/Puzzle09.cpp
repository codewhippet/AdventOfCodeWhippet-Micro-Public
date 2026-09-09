#include "stdafx.h"

using namespace std;

namespace Puzzle09_2020_Types
{
}

using namespace Puzzle09_2020_Types;

static vector<uint32_t> ReadPuzzle()
{
	// Smidge of 'cheating' here: the numbers go into 64-bit range, but we know the answer will lie
	// withing the 32-bit range.
	vector<uint32_t> puzzle;
	puzzle.reserve(1024);

	while (PuzzleInput::NextLine())
	{
		puzzle.push_back(Parse::GetUint32());
	}

	return puzzle;
}

static int32_t FirstEncodingError(const vector<uint32_t>& puzzle)
{
	const size_t windowSize = 25;
	array<uint32_t, windowSize> window = {};

	HashSet<uint32_t> previous(64, numeric_limits<uint32_t>::max());
	for (size_t i = 0; i < puzzle.size(); i++)
	{
		if (i >= windowSize)
		{
			bool isSum = false;
			for (int32_t a : window)
			{
				int32_t b = puzzle[i] - a;
				if (previous.Contains(b))
				{
					isSum = true;
					break;
				}
			}
			if (isSum == false)
			{
				return puzzle[i];
			}

			previous.Erase(window[i % 25]);
		}

		window[i % 25] = puzzle[i];
		previous.Insert(puzzle[i]);
	}

	return -1;
}

void Puzzle09_A_2020()
{
	vector<uint32_t> puzzle = ReadPuzzle();
	int32_t answer = FirstEncodingError(puzzle);

	PuzzleOutput::Submit(2020, 9, 1, answer);
}

void Puzzle09_B_2020()
{
	vector<uint32_t> puzzle = ReadPuzzle();
	const uint32_t target = FirstEncodingError(puzzle);

	size_t i = 0;
	size_t j = 0;
	uint32_t runningSum = 0;

	while (true)
	{
		assert(i <= j);
		assert(i < puzzle.size());
		assert(j < puzzle.size());

		if (runningSum == target)
		{
			break;
		}

		if (runningSum < target)
		{
			runningSum += puzzle[j++];
		}
		else
		{
			assert(runningSum > target);
			runningSum -= puzzle[i++];
		}
	}

	MinMaxValues<int32_t> range;
	for (/***/; i < j; i++)
	{
		range.Update(static_cast<int32_t>(puzzle[i]));
	}

	int32_t answer = range.GetMin() + range.GetMax();

	PuzzleOutput::Submit(2020, 9, 2, answer);
}

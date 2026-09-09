#include "stdafx.h"

using namespace std;

namespace Puzzle10_2020_Types
{
}

using namespace Puzzle10_2020_Types;

static int64_t CountArrangements(const vector<int64_t>& puzzle)
{
	vector<int64_t> dp(puzzle.size());
	dp.back() = 1;

	for (int64_t pos = puzzle.size() - 2; pos >= 0; pos--)
	{
		int64_t arrangements = 0;

		// Look for the next valid adapter
		for (int64_t offset = 1; pos + offset < (int64_t)puzzle.size(); offset++)
		{
			int64_t diff = puzzle[pos + offset] - puzzle[pos];
			if (diff > 3)
			{
				break;
			}

			arrangements += dp[pos + offset];
		}

		dp[pos] = arrangements;
	}

	return dp.front();
}

void Puzzle10_A_2020()
{
	vector<int32_t> puzzle;
	puzzle.reserve(128);
	while (PuzzleInput::NextLine())
	{
		puzzle.push_back(Parse::GetInt32());
	}

	puzzle.push_back(0);
	sort(puzzle.begin(), puzzle.end());
	puzzle.push_back(puzzle.back() + 3);

	int32_t singleDiffs = 0;
	int32_t tripleDiffs = 0;
	for (size_t i = 0; i + 1 < puzzle.size(); i++)
	{
		int32_t diff = puzzle[i + 1] - puzzle[i];
		if (diff == 1)
		{
			singleDiffs++;
		}
		else if (diff == 3)
		{
			tripleDiffs++;
		}
	}

	int32_t answer = singleDiffs * tripleDiffs;

	PuzzleOutput::Submit(2020, 10, 1, answer);
}

void Puzzle10_B_2020()
{
	vector<int64_t> puzzle;
	puzzle.reserve(128);
	while (PuzzleInput::NextLine())
	{
		puzzle.push_back(Parse::GetInt64());
	}

	puzzle.push_back(0);
	sort(puzzle.begin(), puzzle.end());
	puzzle.push_back(puzzle.back() + 3);

	int64_t answer = CountArrangements(puzzle);

	PuzzleOutput::Submit(2020, 10, 2, answer);
}

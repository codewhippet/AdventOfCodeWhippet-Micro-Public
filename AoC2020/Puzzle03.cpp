#include "stdafx.h"

using namespace std;

namespace Puzzle03_2020_Types
{
}

using namespace Puzzle03_2020_Types;

void Puzzle03_A_2020()
{
	vector<string> trees;
	trees.reserve(330);
	Parse::ReadAllLines(&trees, 32);

	size_t toboggan = 0;

	int32_t answer = 0;
	for (size_t row = 0; row < trees.size(); row++)
	{
		answer += (trees[row][toboggan] == '#' ? 1 : 0);
		toboggan = (toboggan + 3) % trees[row].size();
	}

	PuzzleOutput::Submit(2020, 3, 1, answer);
}

void Puzzle03_B_2020()
{
	vector<string> trees;
	trees.reserve(330);
	Parse::ReadAllLines(&trees, 32);

	const vector<Vec2Int> slopes =
	{
		Vec2Int{ 1, 1 },
		Vec2Int{ 3, 1 },
		Vec2Int{ 5, 1 },
		Vec2Int{ 7, 1 },
		Vec2Int{ 1, 2 },
	};

	uint64_t answer = 1;
	for (const Vec2Int& slope : slopes)
	{
		int32_t treesEncountered = 0;

		size_t toboggan = 0;
		for (size_t row = 0; row < trees.size(); row += slope.Y)
		{
			treesEncountered += (trees[row][toboggan] == '#' ? 1 : 0);
			toboggan = (toboggan + slope.X) % trees[row].size();
		}

		answer *= treesEncountered;
	}

	PuzzleOutput::Submit(2020, 3, 2, answer);
}

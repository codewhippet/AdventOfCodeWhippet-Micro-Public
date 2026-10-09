#include "stdafx.h"

using namespace std;

namespace Puzzle09_2021_Types
{
}

using namespace Puzzle09_2021_Types;

static int32_t BasinSize(uArrayMap2D* board, const Vec2Int& start)
{
	const size_t largestSearchQueue = 128;

	vector<Vec2Int> searchQueue;
	searchQueue.reserve(largestSearchQueue);
	searchQueue.push_back(start);

	(*board)(start) = '9';

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		Vec2Int current = searchQueue[i];
		for (const Vec2Int& dir : Vec2Int::CardinalDirections())
		{
			Vec2Int neighbour = current + dir;
			if ((*board)(neighbour) != '9')
			{
				searchQueue.push_back(neighbour);
				(*board)(neighbour) = '9';
			}
		}
	}

	assert(searchQueue.capacity() == largestSearchQueue);
	return static_cast<int32_t>(searchQueue.size());
}

void Puzzle09_A_2021()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 16 * 1024;
	MemArena_Configure(cfg);
	{
		uArrayMap2D board = ReaduArrayMap('9');

		int32_t answer = 0;
		for (const auto& p : board.Grid())
		{
			bool neighboursAllHigher = ranges::all_of(Vec2Int::CardinalDirections(),
				[&](const Vec2Int& dir)
				{
					Vec2Int neighbour = p.first + dir;
					return board(neighbour) > p.second;
				});
			if (neighboursAllHigher)
			{
				answer += (p.second - '0') + 1;
			}
		}

		PuzzleOutput::Submit(2021, 9, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle09_B_2021()
{
	const size_t maxNumBasins = 256;

	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 16 * 1024;
	MemArena_Configure(cfg);
	{
		uArrayMap2D board = ReaduArrayMap('9');

		vector<int32_t> basinSizes;
		basinSizes.reserve(maxNumBasins);
		for (const auto& p : board.Grid())
		{
			if (p.second != '9')
			{
				int32_t basinSize = BasinSize(&board, p.first);
				basinSizes.push_back(basinSize);
			}
		}

		assert(basinSizes.capacity() == maxNumBasins);
		nth_element(basinSizes.begin(), basinSizes.begin() + 3, basinSizes.end(), greater{});

		int32_t answer = basinSizes[0] * basinSizes[1] * basinSizes[2];

		PuzzleOutput::Submit(2021, 9, 2, answer);
	}
	MemArena_Reset();
}

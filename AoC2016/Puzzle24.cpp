#include "stdafx.h"

using namespace std;

namespace Puzzle24_2016_Types
{
	enum : size_t
	{
		NUM_NODES = 8
	};

	using DistanceTo = array<int16_t, NUM_NODES>;
	using DistanceFromTo = array<DistanceTo, NUM_NODES>;
}

using namespace Puzzle24_2016_Types;

static Vec2Short Compress(const Vec2Int& v)
{
	return { static_cast<int16_t>(v.X), static_cast<int16_t>(v.Y) };
}

static Vec2Int Decompress(const Vec2Short& v)
{
	return { v.X, v.Y };
}

static void DistanceBetweenNodes(const uArrayMap2D& grid, const Vec2Int& startFrom, DistanceTo* distanceTo)
{
	uvector<pair<Vec2Short, int16_t>> searchQueue;
	size_t reserveSize = ((grid.GetWidth() * grid.GetHeight()) * 5) / 8; // Excluding walls, we're likely to cover just over half of the map
	searchQueue.reserve(reserveSize);
	searchQueue.push_back({ Compress(startFrom), int16_t(0) });

	uArrayMap2D queued(grid);

	assert(isdigit(grid(startFrom)));
	size_t startNode = grid(startFrom) - '0';
	(*distanceTo)[startNode] = 0;
	queued(startFrom) = '#';

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		Vec2Int currentPos = Decompress(searchQueue[i].first);
		int16_t currentSteps = searchQueue[i].second;
		char currentNode = grid(currentPos);
		if (isdigit(currentNode))
		{
			(*distanceTo)[currentNode - '0'] = currentSteps;
		}

		for (const Vec2Int& dir : Vec2Int::CardinalDirections())
		{
			Vec2Int neighbour = currentPos + dir;
			if (queued(neighbour) != '#')
			{
				searchQueue.push_back({ Compress(neighbour), static_cast<int16_t>(currentSteps + 1) });
				queued(neighbour) = '#';
			}
		}
	}
}

static void DistancesBetweenAllNodes(const uArrayMap2D& grid, DistanceFromTo* distances)
{
	array<Vec2Int, NUM_NODES> nodeLocations;
	for (const auto& p : grid.Grid())
	{
		if (isdigit(p.second))
		{
			nodeLocations[p.second - '0'] = p.first;
		}
	}

	for (size_t i = 0; i < NUM_NODES; i++)
	{
		DistanceBetweenNodes(grid, nodeLocations[i], &(*distances)[i]);
	}
}

void Puzzle24_A_2016()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 64 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D grid = ReaduArrayMap();

		DistanceFromTo distances;
		DistancesBetweenAllNodes(grid, &distances);

		int32_t answer = numeric_limits<int16_t>::max();
		array<size_t, NUM_NODES> order{ 0, 1, 2, 3, 4, 5, 6, 7 };
		for (size_t i = 0; i < order.size(); i++)
		{
			order[i] = i;
		}

		do
		{
			int16_t distance = 0;
			for (size_t i = 1; i < order.size(); i++)
			{
				distance += distances[order[i - 1]][order[i]];
			}
			if (distance < answer)
			{
				answer = distance;
			}
		}
		while (next_permutation(order.begin() + 1, order.end()));

		PuzzleOutput::Submit(2016, 24, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle24_B_2016()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 64 * 1024;
	cfg.DebugFlags = MemArenaDebugFlags::PrintOnOutOfMemory | MemArenaDebugFlags::BreakOnOutOfMemory;

	MemArena_Configure(cfg);
	{
		uArrayMap2D grid = ReaduArrayMap();

		DistanceFromTo distances;
		DistancesBetweenAllNodes(grid, &distances);

		int32_t answer = numeric_limits<int16_t>::max();
		array<size_t, NUM_NODES> order{ 0, 1, 2, 3, 4, 5, 6, 7 };
		for (size_t i = 0; i < order.size(); i++)
		{
			order[i] = i;
		}

		do
		{
			int16_t distance = 0;
			for (size_t i = 1; i < order.size(); i++)
			{
				distance += distances[order[i - 1]][order[i]];
			}
			distance += distances[order.back()][0];

			if (distance < answer)
			{
				answer = distance;
			}
		}
		while (next_permutation(order.begin() + 1, order.end()));

		PuzzleOutput::Submit(2016, 24, 2, answer);
	}
	MemArena_Reset();
}

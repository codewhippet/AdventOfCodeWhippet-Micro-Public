#include "stdafx.h"

using namespace std;

namespace Puzzle22_2016_Types
{
	struct Node
	{
		Vec2Int Pos = { 0, 0 };
		int16_t Size = 0;
		int16_t Used = 0;
		int16_t Avail = 0;
	};
}

using namespace Puzzle22_2016_Types;

static Node* ParseNodes(size_t* nodeCount)
{
	size_t allocatedBytes;
	Node* nodes = static_cast<Node*>(MemArena_AllocLargest(&allocatedBytes));
	size_t maxNodes = allocatedBytes / sizeof(Node);
	(void)maxNodes;

	Node* dest = nodes;
	while (PuzzleInput::NextLine())
	{
		dest->Pos.X = Parse::GetInt32();
		dest->Pos.Y = Parse::GetInt32();
		dest->Size = Parse::GetInt16();
		dest->Used = Parse::GetInt16();
		dest->Avail = Parse::GetInt16();
		dest++;

		PuzzleInput::DropLine();
	}
	*nodeCount = dest - nodes;
	assert(*nodeCount <= maxNodes);
	MemArena_ShrinkAlloc(nodes, sizeof(Node) * *nodeCount);

	return nodes;
}

static int32_t ShortestPathToMoveHoleToGoal(uArrayMap2D* grid, const Vec2Int& emptyNode, const Vec2Int& goalData)
{
	uvector<pair<int32_t, Vec2Int>> searchQueue;
	searchQueue.reserve(1024);
	searchQueue.push_back({ 0, emptyNode });

	for (size_t i = 0; i < searchQueue.size(); i++)
	{
		Vec2Int currentPos = searchQueue[i].second;
		int32_t currentSteps = searchQueue[i].first;

		if (currentPos == goalData)
		{
			return currentSteps;
		}

		for (const Vec2Int& dir : Vec2Int::CardinalDirections())
		{
			Vec2Int neighbour = currentPos + dir;
			if ((*grid)(neighbour) != '#')
			{
				searchQueue.push_back({ currentSteps + 1, neighbour });
				(*grid)(neighbour) = '#';
			}
		}
	}

	return -1;
}

void Puzzle22_A_2016()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 16 * 1024;

	MemArena_Configure(cfg);
	{
		size_t nodeCount = numeric_limits<size_t>::max();
		Node* nodes = ParseNodes(&nodeCount);

		int32_t answer = 0;
		for (size_t i = 0; i < nodeCount; i++)
		{
			for (size_t j = i + 1; j < nodeCount; j++)
			{
				answer += ((nodes[i].Used) && (nodes[i].Used <= nodes[j].Avail) ? 1 : 0);
				answer += ((nodes[j].Used) && (nodes[j].Used <= nodes[i].Avail) ? 1 : 0);
			}
		}

		PuzzleOutput::Submit(2016, 22, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle22_B_2016()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 32 * 1024;
	cfg.DebugFlags = MemArenaDebugFlags::PrintOnOutOfMemory | MemArenaDebugFlags::BreakOnOutOfMemory;

	MemArena_Configure(cfg);
	{
		size_t nodeCount = numeric_limits<size_t>::max();
		Node* nodes = ParseNodes(&nodeCount);
		auto nodeRange = ranges::subrange(nodes, nodes + nodeCount);

		Vec2Int emptyNode = (nodeRange | views::filter([](const Node& n) { return n.Used == 0; })).front().Pos;
		int16_t smallestHdd = ranges::min(nodeRange | views::transform([](const Node& n) { return n.Size; }));
		int32_t maximumX = ranges::max(nodeRange | views::transform([](const Node& n) { return n.Pos.X; }));
		int32_t maximumY = ranges::max(nodeRange | views::transform([](const Node& n) { return n.Pos.Y; }));
		Vec2Int goalData{ maximumX, 0 };

		uArrayMap2D grid({ 0, 0 }, maximumX + 1, maximumY + 1, '#');
		for (const Node& n : nodeRange)
		{
			if (n.Used <= smallestHdd)
			{
				grid(n.Pos) = '.';
			}
		}

		int32_t moveHoleToGoal = ShortestPathToMoveHoleToGoal(&grid, emptyNode, goalData);

		// It takes 5 moves to shuffle the goal data forward by 1 square. We've already moved the
		// goal data forward by 1 by placing the hole on top of the starting location.
		int32_t answer = moveHoleToGoal + (5 * (goalData.X - 1));

		PuzzleOutput::Submit(2016, 22, 2, answer);
	}
	MemArena_Reset();
}

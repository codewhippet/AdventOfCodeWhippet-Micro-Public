#include "stdafx.h"

using namespace std;

namespace Puzzle23_2018_Types
{
	struct Nanobot
	{
		Vec3Int Position;
		int32_t Radius = -1;
	};

	struct AABB
	{
		Vec3Int Min;
		Vec3Int Max;
		int32_t LogSize;
	};

	struct OctreeSearchNode
	{
		int32_t BotsInRange;
		AABB Bounds;
	};
}

using namespace Puzzle23_2018_Types;

static int32_t Distance(int32_t p, int32_t low, int32_t hi)
{
	if (p < low) return low - p;
	if (p > hi) return p - hi;
	return 0;
}

static int32_t BotsInRange(const vector<Nanobot>& nanobots, const AABB& aabb)
{
	int32_t inRange = 0;
	for (const Nanobot& n : nanobots)
	{
		int32_t distanceX = Distance(n.Position.X, aabb.Min.X, aabb.Max.X);
		if (distanceX > n.Radius)
			continue;

		int32_t distanceY = Distance(n.Position.Y, aabb.Min.Y, aabb.Max.Y);
		if (distanceY > n.Radius)
			continue;

		int32_t distanceZ = Distance(n.Position.Z, aabb.Min.Z, aabb.Max.Z);
		if (distanceZ > n.Radius)
			continue;

		// 3 * n.Radius should be small enough not to overflow
		if ((distanceX + distanceY + distanceZ) <= n.Radius)
		{
			inRange++;
		}
	}
	return inRange;
}

static void Split(const AABB& parent, SmallVector<AABB, 8>* children)
{
	assert(parent.LogSize > 0);

	const int32_t newLogSize = parent.LogSize - 1;
	const int32_t splitMask = (1 << newLogSize) - 1;

	const pair<int32_t, int32_t> boundX[2] = { { parent.Min.X & ~splitMask, parent.Min.X | splitMask }, { parent.Max.X & ~splitMask, parent.Max.X | splitMask } };
	const pair<int32_t, int32_t> boundY[2] = { { parent.Min.Y & ~splitMask, parent.Min.Y | splitMask }, { parent.Max.Y & ~splitMask, parent.Max.Y | splitMask } };
	const pair<int32_t, int32_t> boundZ[2] = { { parent.Min.Z & ~splitMask, parent.Min.Z | splitMask }, { parent.Max.Z & ~splitMask, parent.Max.Z | splitMask } };

	for (int32_t z = 0; z < 2; z++)
	{
		for (int32_t y = 0; y < 2; y++)
		{
			for (int32_t x = 0; x < 2; x++)
			{
				AABB child;
				child.LogSize = newLogSize;
				child.Min = Vec3Int{ boundX[x].first, boundY[y].first, boundZ[z].first };
				child.Max = Vec3Int{ boundX[x].second, boundY[y].second, boundZ[z].second };
				children->PushBack(child);
			}
		}
	}
}

void Puzzle23_A_2018()
{
	vector<Nanobot> nanobots;
	nanobots.reserve(1024);

	Nanobot largestRadius;
	while (PuzzleInput::NextLine())
	{
		Nanobot n;
		n.Position.X = Parse::GetInt32();
		n.Position.Y = Parse::GetInt32();
		n.Position.Z = Parse::GetInt32();
		n.Radius = Parse::GetInt32();
		nanobots.push_back(n);

		if (n.Radius > largestRadius.Radius)
		{
			largestRadius = n;
		}
	}

	int32_t answer = static_cast<int32_t>(ranges::count_if(nanobots, [&](const Nanobot& n) { return ManhattanDistance(largestRadius.Position, n.Position) <= largestRadius.Radius; }));
	PuzzleOutput::Submit(2018, 23, 1, answer);
}

void Puzzle23_B_2018()
{
	vector<Nanobot> nanobots;
	nanobots.reserve(1024);

	while (PuzzleInput::NextLine())
	{
		Nanobot n;
		n.Position.X = Parse::GetInt32();
		n.Position.Y = Parse::GetInt32();
		n.Position.Z = Parse::GetInt32();
		n.Radius = Parse::GetInt32();
		nanobots.push_back(n);
	}

	vector<OctreeSearchNode> searchQueue;
	searchQueue.reserve(1024);

	{
		OctreeSearchNode startingNode;
		startingNode.Bounds.LogSize = 32;
		startingNode.Bounds.Min = Vec3Int{ numeric_limits<int32_t>::min(), numeric_limits<int32_t>::min(), numeric_limits<int32_t>::min() };
		startingNode.Bounds.Max = Vec3Int{ numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() };
		startingNode.BotsInRange = BotsInRange(nanobots, startingNode.Bounds);
		assert(startingNode.BotsInRange == nanobots.size());
		searchQueue.push_back(startingNode);
		ranges::push_heap(searchQueue, {}, &OctreeSearchNode::BotsInRange);
	}

	int32_t answer = 0;
	while (searchQueue.size() > 0)
	{
		const OctreeSearchNode& top = searchQueue.front();
		if (top.Bounds.LogSize == 0)
		{
			answer = ManhattanDistance(top.Bounds.Min, {});
			break;
		}

		SmallVector<AABB, 8> splitBounds;
		Split(top.Bounds, &splitBounds);

		ranges::pop_heap(searchQueue, {}, &OctreeSearchNode::BotsInRange);
		searchQueue.pop_back();

		for (size_t i = 0; i < splitBounds.size(); i++)
		{
			OctreeSearchNode childNode;
			childNode.Bounds = splitBounds[i];
			childNode.BotsInRange = BotsInRange(nanobots, childNode.Bounds);
			if (childNode.BotsInRange > 0)
			{
				searchQueue.push_back(childNode);
				ranges::push_heap(searchQueue, {}, &OctreeSearchNode::BotsInRange);
			}
		}
	}

	PuzzleOutput::Submit(2018, 23, 2, answer);
}

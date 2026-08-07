#include "stdafx.h"

using namespace std;

namespace Puzzle23_2018_Types
{
	enum class Side : int32_t
	{
		Begin,
		End
	};

	struct Nanobot
	{
		Vec3Int Position;
		int32_t Radius = -1;
	};

	struct Octahedron
	{
		int32_t PlanesBegin[4] = {};
		int32_t PlanesEnd[4] = {};
	};
}

using namespace Puzzle23_2018_Types;

static bool TryIntersect(const Octahedron& a, Octahedron* b)
{
	Octahedron c = a;

	for (int i = 0; i < 4; i++)
	{
		c.PlanesBegin[i] = max(c.PlanesBegin[i], b->PlanesBegin[i]);
		c.PlanesEnd[i] = min(c.PlanesEnd[i], b->PlanesEnd[i]);
		if (c.PlanesEnd[i] < c.PlanesBegin[i])
		{
			return false;
		}
	}

	*b = c;
	return true;
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
	const Vec3Int topNormals[] =
	{
		{  1,  1,  1 },
		{ -1,  1,  1 },
		{  1, -1,  1 },
		{ -1, -1,  1 },
	};

	vector<Octahedron> nanobotBounds;
	nanobotBounds.reserve(1024);

	while (PuzzleInput::NextLine())
	{
		Nanobot n;
		n.Position.X = Parse::GetInt32();
		n.Position.Y = Parse::GetInt32();
		n.Position.Z = Parse::GetInt32();
		n.Radius = Parse::GetInt32();

		Vec3Int top = n.Position + (Vec3Int{ 0, 0, 1 } * n.Radius);
		Vec3Int bottom = n.Position + (Vec3Int{ 0, 0, -1 } * n.Radius);

		Octahedron bounds;
		for (int i = 0; i < 4; i++)
		{
			bounds.PlanesBegin[i] = Dot(topNormals[i], bottom);
			bounds.PlanesEnd[i] = Dot(topNormals[i], top);
			assert(bounds.PlanesBegin[i] < bounds.PlanesEnd[i]);
		}
		nanobotBounds.push_back(bounds);
	}

	vector<int32_t> candidateSet;
	for (size_t side = 0; side < 4; side++)
	{
		vector<tuple<int32_t, Side, int32_t>> edges;
		edges.reserve(2 * 1024);
		for (int32_t botIndex = 0; botIndex < static_cast<int32_t>(nanobotBounds.size()); botIndex++)
		{
			const Octahedron& bounds = nanobotBounds[botIndex];
			edges.push_back({ bounds.PlanesBegin[side], Side::Begin, botIndex });
			edges.push_back({ bounds.PlanesEnd[side], Side::End, botIndex });
		}
		ranges::sort(edges);

		vector<int32_t> biggestSet;
		biggestSet.reserve(1024);
		{
			HashSet<int32_t> activeRanges(2 * 1024, -1);
			for (const auto& kvp : edges)
			{
				int32_t bot = get<2>(kvp);
				if (get<1>(kvp) == Side::Begin)
				{
					assert(activeRanges.Contains(bot) == false);
					activeRanges.Insert(bot);
				}
				else
				{
					if (activeRanges.Size() > biggestSet.size())
					{
						biggestSet.clear();
						ranges::copy(activeRanges, back_inserter(biggestSet));
					}

					assert(activeRanges.Contains(bot));
					bool erased = activeRanges.Erase(bot);
					assert(erased);
					(void)erased;
				}
			}
		}

		ranges::sort(biggestSet);
		if (candidateSet.empty())
		{
			candidateSet.swap(biggestSet);
		}
		else
		{
			vector<int32_t> intersection;
			intersection.reserve(1024);

			ranges::set_intersection(candidateSet, biggestSet, inserter(intersection, intersection.end()));
			candidateSet.swap(intersection);
		}
	}

	Octahedron candidateBounds =
	{
		{ numeric_limits<int32_t>::min(), numeric_limits<int32_t>::min(), numeric_limits<int32_t>::min(), numeric_limits<int32_t>::min() },
		{ numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() },
	};
	for (int32_t botIndex : candidateSet)
	{
		bool intersected = TryIntersect(nanobotBounds[botIndex], &candidateBounds);
		assert(intersected);
		(void)intersected;
	}

	int32_t answer = 0;
	for (int i = 0; i < 4; i++)
	{
		if (candidateBounds.PlanesEnd[i] < 0)
		{
			answer = max(answer, -candidateBounds.PlanesEnd[i]);
		}
		else if (candidateBounds.PlanesBegin[i] > 0)
		{
			answer = max(answer, candidateBounds.PlanesBegin[i]);
		}
	}

	PuzzleOutput::Submit(2018, 23, 2, answer);
}

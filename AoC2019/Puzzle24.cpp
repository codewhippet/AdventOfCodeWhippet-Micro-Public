#include "stdafx.h"
#include <span>

using namespace std;

namespace Puzzle24_2019_Types
{
}

using namespace Puzzle24_2019_Types;

static int32_t GridIndex(const Vec3Int& v)
{
	assert(v.Z == 0);
	return (v.Y * 5) + v.X;
}

static int32_t OffsetFromTo(const Vec3Int& from, const Vec3Int& to)
{
	int32_t offset = 0;
	offset += (to.X - from.X) * 1;
	offset += (to.Y - from.Y) * 5;
	offset += (to.Z - from.Z) * 25;
	return offset;
}

static vector<SmallVector<int32_t, 8>> BuildRecursiveNeighbours()
{
	vector<SmallVector<int32_t, 8>> neighbours;
	neighbours.resize(25);

	constexpr Vec3Int G01{ 0, 0, 0 };
	constexpr Vec3Int G02{ 1, 0, 0 };
	constexpr Vec3Int G03{ 2, 0, 0 };
	constexpr Vec3Int G04{ 3, 0, 0 };
	constexpr Vec3Int G05{ 4, 0, 0 };

	constexpr Vec3Int G06{ 0, 1, 0 };
	constexpr Vec3Int G07{ 1, 1, 0 };
	constexpr Vec3Int G08{ 2, 1, 0 };
	constexpr Vec3Int G09{ 3, 1, 0 };
	constexpr Vec3Int G10{ 4, 1, 0 };

	constexpr Vec3Int G11{ 0, 2, 0 };
	constexpr Vec3Int G12{ 1, 2, 0 };
	// G13
	constexpr Vec3Int G14{ 3, 2, 0 };
	constexpr Vec3Int G15{ 4, 2, 0 };

	constexpr Vec3Int G16{ 0, 3, 0 };
	constexpr Vec3Int G17{ 1, 3, 0 };
	constexpr Vec3Int G18{ 2, 3, 0 };
	constexpr Vec3Int G19{ 3, 3, 0 };
	constexpr Vec3Int G20{ 4, 3, 0 };

	constexpr Vec3Int G21{ 0, 4, 0 };
	constexpr Vec3Int G22{ 1, 4, 0 };
	constexpr Vec3Int G23{ 2, 4, 0 };
	constexpr Vec3Int G24{ 3, 4, 0 };
	constexpr Vec3Int G25{ 4, 4, 0 };

	constexpr Vec3Int Inner{ 0, 0, 1 };
	constexpr Vec3Int Outer{ 0, 0, -1 };

	const vector<pair<Vec3Int, vector<Vec3Int>>> samePlaneAdjacencies =
	{
		{ G01, { G02, G06 } },
		{ G02, { G03, G07 } },
		{ G03, { G04, G08 } },
		{ G04, { G05, G09 } },
		{ G05, { G10 } },

		{ G06, { G07, G11 } },
		{ G07, { G08, G12 } },
		{ G08, { G09 } },
		{ G09, { G10, G14 } },
		{ G10, { G15 } },

		{ G11, { G12, G16 } },
		{ G12, { G17 } },
		// G13
		{ G14, { G15, G19 } },
		{ G15, { G20 } },

		{ G16, { G17, G21 } },
		{ G17, { G18, G22 } },
		{ G18, { G19, G23 } },
		{ G19, { G20, G24 } },
		{ G20, { G25 } },

		{ G21, { G22 } },
		{ G22, { G23 } },
		{ G23, { G24 } },
		{ G24, { G25 } },
	};

	for (const auto& samePlane : samePlaneAdjacencies)
	{
		for (const auto& link : samePlane.second)
		{
			neighbours[GridIndex(samePlane.first)].PushBack(OffsetFromTo(samePlane.first, link));
			neighbours[GridIndex(link)].PushBack(OffsetFromTo(link, samePlane.first));
		}
	}

	const vector<pair<Vec3Int, vector<Vec3Int>>> innerOuterAdjacencies =
	{
		{ G08, { G01, G02, G03, G04, G05 } },
		{ G12, { G01, G06, G11, G16, G21 } },
		{ G14, { G05, G10, G15, G20, G25 } },
		{ G18, { G21, G22, G23, G24, G25 } },
	};

	for (const auto& differentPlane : innerOuterAdjacencies)
	{
		for (const auto& link : differentPlane.second)
		{
			neighbours[GridIndex(differentPlane.first)].PushBack(OffsetFromTo(differentPlane.first, link + Inner));
			neighbours[GridIndex(link)].PushBack(OffsetFromTo(link, differentPlane.first + Outer));
		}
	}

	return neighbours;
}

static void Step(const uArrayMap2D& oldEris, uArrayMap2D* newEris)
{
	for (const auto& p : oldEris.Grid())
	{
		int32_t bugCount = static_cast<int32_t>(ranges::count(Vec2Int::CardinalDirections() | views::transform([&](const Vec2Int& dir) { return oldEris(p.first + dir); }), '#'));

		if ((p.second == '#') && (bugCount != 1))
		{
			// A bug dies (becoming an empty space) unless there is exactly one bug adjacent to it.
			(*newEris)(p.first) = '.';
		}
		else if ((p.second == '.') && ((bugCount == 1) || (bugCount == 2)))
		{
			// An empty space becomes infested with a bug if exactly one or two bugs are adjacent to it.
			(*newEris)(p.first) = '#';
		}
		else
		{
			// No change
			(*newEris)(p.first) = p.second;
		}
	}
}

static int32_t CalculateBiodiversity(const uArrayMap2D& eris)
{
	int32_t biodiversity = 0;
	for (size_t i = 0; i < eris.GetData().size(); i++)
	{
		if (eris.GetData()[i] == '#')
		{
			biodiversity |= 1 << i;
		}
	}
	return biodiversity;
}

static int32_t FindFirstRepeat(const vector<char>& erisStart)
{
	HashSet<int32_t> seen(64, 0xffffffff);

	array<uArrayMap2D, 2> erises{
		uArrayMap2D{ CreateArrayMap2DAllocator_Heap(), {}, 5, 5, '.'},
		uArrayMap2D{ CreateArrayMap2DAllocator_Heap(), {}, 5, 5, '.'}
	};

	assert(erises[0].GetData().size() == erisStart.size());
	memcpy(erises[0].GetData().data(), erisStart.data(), erisStart.size());

	size_t currentEris = 0;
	while (true)
	{
		int32_t biodiversity = CalculateBiodiversity(erises[currentEris]);
		if (seen.Insert(biodiversity) == false)
			return biodiversity;

		Step(erises[currentEris], &erises[1 - currentEris]);
		currentEris = 1 - currentEris;
	}
}

static void StepRecursive(const vector<int32_t>& oldErises,
	const vector<SmallVector<int32_t, 8>>& neighbours,
	int32_t updateBegin,
	int32_t updateEnd,
	vector<int32_t>* newErises)
{
	for (int32_t cellIndex = updateBegin; cellIndex < updateEnd; cellIndex++)
	{
		int32_t gridIndex = cellIndex % 25;

		int32_t bugCount = static_cast<int32_t>(ranges::count_if(neighbours[gridIndex], [&](int32_t offset) { return oldErises[cellIndex + offset] == '#'; }));

		int32_t current = oldErises[cellIndex];

		if ((current == '#') && (bugCount != 1))
		{
			// A bug dies (becoming an empty space) unless there is exactly one bug adjacent to it.
			(*newErises)[cellIndex] = '.';
		}
		else if ((current == '.') && ((bugCount == 1) || (bugCount == 2)))
		{
			// An empty space becomes infested with a bug if exactly one or two bugs are adjacent to it.
			(*newErises)[cellIndex] = '#';
		}
		else
		{
			// No change
			(*newErises)[cellIndex] = current;
		}
	}
}

void Puzzle24_A_2019()
{
	vector<char> erisStart;
	erisStart.reserve(25);

	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		if (c != '\n')
		{
			erisStart.push_back(static_cast<char>(c));
		}
	}

	int32_t answer = FindFirstRepeat(erisStart);

	PuzzleOutput::Submit(2019, 24, 1, answer);
}

void Puzzle24_B_2019()
{
	vector<char> erisStart;
	erisStart.reserve(25);

	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		if (c != '\n')
		{
			erisStart.push_back(static_cast<char>(c));
		}
	}

	vector<SmallVector<int32_t, 8>> neighbours = BuildRecursiveNeighbours();
	(void)neighbours;

	array<vector<int32_t>, 2> erises;
	erises[0].resize(403 * 25, '.');
	erises[1].resize(403 * 25, '.');

	int32_t middleEris = 201 * 25;
	for (size_t i = 0; i < 25; i++)
	{
		erises[0][middleEris + i] = erisStart[i];
	}

	int32_t currentEris = 0;
	for (int32_t step = 0; step < 200; step++)
	{
		int32_t updateBegin = (201 - step - 1) * 25;
		int32_t updateEnd = (201 + step + 2) * 25;

		StepRecursive(erises[currentEris], neighbours, updateBegin, updateEnd, &erises[1 - currentEris]);

		currentEris = 1 - currentEris;
	}

	int32_t answer = static_cast<int32_t>(ranges::count(erises[currentEris], '#'));

	PuzzleOutput::Submit(2019, 24, 2, answer);
}

#include "stdafx.h"

using namespace std;

namespace Puzzle25_2018_Types
{
	struct Constellation
	{
		Constellation* Parent = nullptr;
		Vec4Int Star;

		Constellation* GetRepresentative()
		{
			Constellation* representative = this;
			while (representative->Parent)
				representative = representative->Parent;
			return representative;
		}
	};
}

using namespace Puzzle25_2018_Types;

static bool TryMerge(Constellation* a, Constellation* b)
{
	a = a->GetRepresentative();
	b = b->GetRepresentative();
	if (a == b)
	{
		return false;
	}

	b->Parent = a;
	return true;
}

static int64_t ManhattanDistance(const Vec4Int& a, const Vec4Int& b)
{
	return abs(b.X - a.X) + abs(b.Y - a.Y) + abs(b.Z - a.Z) + abs(b.W - a.W);
}

void Puzzle25_A_2018()
{
	vector<Constellation> stars;
	stars.reserve(1500);

	while (PuzzleInput::NextLine())
	{
		stars.push_back({ nullptr, Vec4Int{ Parse::GetInt32(), Parse::GetInt32(), Parse::GetInt32(), Parse::GetInt32() } });
	}

	const int32_t maximumDistance = 3;
	for (const auto& p : AllUnorderedPairs(stars.size()))
	{
		if (ManhattanDistance(stars[p.first].Star, stars[p.second].Star) <= maximumDistance)
		{
			TryMerge(&stars[p.first], &stars[p.second]);
		}
	}

	HashSet<Constellation*> constellations(1024, nullptr);
	for (auto& star : stars)
	{
		constellations.Insert(star.GetRepresentative());
	}

	int32_t answer = static_cast<int32_t>(constellations.Size());
	PuzzleOutput::Submit(2018, 25, 1, answer);
}

void Puzzle25_B_2018()
{
	return PuzzleOutput::Submit(2018, 25, 2, int32_t(-1));
}

#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle03_2018_Types
{
	struct Claim
	{
		Vec2Int TopLeft;
		Vec2Int BottomRight;
		int32_t Id;
		bool Overlaps = false;
	};

	struct Fabric
	{
		Vec2Int TopLeft;
		Vec2Int BottomRight;
		vector<Claim> Claims;
	};

	enum class EdgeType : uint32_t
	{
		Open, // Open must sort before Close
		Close,
	};

	struct Edge
	{
		int32_t X;
		EdgeType Type;
		int32_t Id;

		auto operator<=>(const Edge&) const = default;
	};
}

using namespace Puzzle03_2018_Types;

static Fabric ParseClaims()
{
	Fabric ret;
	ret.Claims.reserve(1300);
	ret.TopLeft = Vec2Int::Max();
	ret.BottomRight = Vec2Int::Min();

	while (PuzzleInput::NextLine())
	{
		Claim c;
		c.Id = Parse::GetInt32();
		c.TopLeft = { Parse::GetInt32(), Parse::GetInt32() };
		c.BottomRight = c.TopLeft + Vec2Int{ Parse::GetInt32(), Parse::GetInt32() } - Vec2Int{ 1, 1 };
		ret.Claims.push_back(c);

		assert(c.Id == ret.Claims.size());

		ret.TopLeft = Vec2Int::MinElements(ret.TopLeft, c.TopLeft);
		ret.BottomRight = Vec2Int::MaxElements(ret.BottomRight, c.BottomRight);
	}

	return ret;
}

static size_t CountDuplicates(const Fabric& fabric, int32_t line)
{
	int32_t fabricOffset = fabric.TopLeft.X;
	int32_t fabricWidth = fabric.BottomRight.X - fabric.TopLeft.X + 1;
	vector<int32_t> claimCounts(fabricWidth);

	for (const Claim& c : fabric.Claims)
	{
		if ((line >= c.TopLeft.Y) && (line <= c.BottomRight.Y))
		{
			for (int32_t x = c.TopLeft.X; x <= c.BottomRight.X; x++)
			{
				claimCounts[x - fabricOffset]++;
			}
		}
	}

	return ranges::count_if(claimCounts, [](int32_t c) { return c >= 2; });
}

static bool DoesOverlap(const Claim& a, const Claim& b)
{
	bool disjoint =
		(a.BottomRight.X < b.TopLeft.X) ||
		(a.BottomRight.Y < b.TopLeft.Y) ||
		(b.BottomRight.X < a.TopLeft.X) ||
		(b.BottomRight.Y < a.TopLeft.Y);
	return !disjoint;
}

void Puzzle03_A_2018()
{
	Fabric fabric = ParseClaims();

	size_t answer = 0;
	for (int32_t y = fabric.TopLeft.Y; y <= fabric.BottomRight.Y; y++)
	{
		answer += CountDuplicates(fabric, y);
	}

	PuzzleOutput::Submit(2018, 3, 1, answer);
}

void Puzzle03_B_2018()
{
	Fabric fabric = ParseClaims();

	vector<Edge> edges;
	edges.reserve(fabric.Claims.size() * 2);
	for (const Claim& c : fabric.Claims)
	{
		edges.push_back({ c.TopLeft.X, EdgeType::Open, c.Id });
		edges.push_back({ c.BottomRight.X, EdgeType::Close, c.Id });
	}
	ranges::sort(edges);

	vector<int32_t> openRanges;
	openRanges.reserve(fabric.Claims.size());
	for (const Edge& e : edges)
	{
		if (e.Type == EdgeType::Open)
		{
			Claim& claimA = fabric.Claims[e.Id - 1];
			for (uint32_t claimId : openRanges)
			{
				Claim& claimB = fabric.Claims[claimId - 1];
				if (DoesOverlap(claimA, claimB))
				{
					claimA.Overlaps = true;
					claimB.Overlaps = true;
				}
			}

			openRanges.push_back(e.Id);
		}
		else
		{
			assert(e.Type == EdgeType::Close);
			auto existingIt = ranges::find(openRanges, e.Id);
			swap(*existingIt, openRanges.back());
			openRanges.resize(openRanges.size() - 1);
		}
	}

	int32_t answer = (fabric.Claims | views::filter([](const Claim& c) { return c.Overlaps == false; })).front().Id;
	PuzzleOutput::Submit(2018, 3, 2, answer);
}

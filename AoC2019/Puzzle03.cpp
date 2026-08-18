#include "stdafx.h"

using namespace std;

namespace Puzzle03_2019_Types
{
	enum RangeType : int32_t
	{
		// One of either A or B must nest inside the other
		OpenA,
		OpenB,
		CloseB,
		CloseA,
	};

	struct WireSegment
	{
		Vec2Int From;
		Vec2Int To;

		Vec2Int Min;
		Vec2Int Max;

		int32_t Steps;
	};

	struct Intersection
	{
		Vec2Int Pos;
		int32_t CombinedSteps;
	};
}

using namespace Puzzle03_2019_Types;

static vector<WireSegment> WireSegments()
{
	vector<WireSegment> segments;
	segments.reserve(301);

	array<Vec2Int, 26> directions;
	directions['U' - 'A'] = Vec2Int::Up();
	directions['D' - 'A'] = Vec2Int::Down();
	directions['L' - 'A'] = Vec2Int::Left();
	directions['R' - 'A'] = Vec2Int::Right();

	Vec2Int current{};
	int32_t steps = 0;
	while (true)
	{
		Vec2Int dir = directions[PuzzleInput::GetChar() - 'A'];
		int32_t dist = Parse::GetInt32();
		Vec2Int next = current + dir * dist;

		segments.push_back({ current, next, Vec2Int::MinElements(current, next), Vec2Int::MaxElements(current, next), steps });

		current = next;
		steps += dist;

		if (PuzzleInput::GetChar() == '\n')
			break;
	}

	return segments;
}

static vector<tuple<int32_t, RangeType, int32_t>> SortedRanges(const vector<WireSegment>& wiresA, const vector<WireSegment>& wiresB)
{
	vector<tuple<int32_t, RangeType, int32_t>> ranges;
	ranges.reserve(wiresA.size() * 2 + wiresB.size() * 2);
	for (int32_t i = 0; i < static_cast<int32_t>(wiresA.size()); i++)
	{
		const WireSegment& segment = wiresA[i];
		ranges.push_back({ segment.Min.X, RangeType::OpenA, i });
		ranges.push_back({ segment.Max.X, RangeType::CloseA, i });
	}
	for (int32_t i = 0; i < static_cast<int32_t>(wiresB.size()); i++)
	{
		const WireSegment& segment = wiresB[i];
		ranges.push_back({ segment.Min.X, RangeType::OpenB, i });
		ranges.push_back({ segment.Max.X, RangeType::CloseB, i });
	}
	ranges::sort(ranges);
	return ranges;
}

static int32_t Clamp(int32_t value, int32_t minValue, int32_t maxValue)
{
	return max(minValue, min(value, maxValue));
}

static bool TryIntersect(const WireSegment& a, const WireSegment& b, Intersection* intersection)
{
	if (a.Max.Y < b.Min.Y)
		return false;
	if (b.Max.Y < a.Min.Y)
		return false;

	Vec2Int p{};

	p.X = Clamp(p.X, max(a.Min.X, b.Min.X), min(a.Max.X, b.Max.X));
	p.Y = Clamp(p.Y, max(a.Min.Y, b.Min.Y), min(a.Max.Y, b.Max.Y));

	intersection->Pos = p;
	intersection->CombinedSteps =
		abs(p.X - a.From.X) + abs(p.X - b.From.X) +
		abs(p.Y - a.From.Y) + abs(p.Y - b.From.Y) +
		a.Steps +
		b.Steps;
	return true;
}

static void FindIntersections(const vector<WireSegment>& wires, const vector<int32_t>& activeWires, const WireSegment& segment, vector<Intersection>* intersections)
{
	for (int32_t wireIdx : activeWires)
	{
		Intersection intersection;
		if (TryIntersect(wires[wireIdx], segment, &intersection))
		{
			intersections->push_back(intersection);
		}
	}
}

static vector<Intersection> FindAllIntersections(const vector<WireSegment>& wiresA,
	const vector<WireSegment>& wiresB,
	const vector<tuple<int32_t, RangeType, int32_t>>& ranges)
{
	vector<Intersection> intersections;
	intersections.reserve(32);

	vector<int32_t> activeA;
	activeA.reserve(32);

	vector<int32_t> activeB;
	activeB.reserve(32);

	for (int32_t i = 0; i < static_cast<int32_t>(ranges.size()); i++)
	{
		int32_t wireIndex = get<2>(ranges[i]);
		switch (get<1>(ranges[i]))
		{
		case RangeType::OpenA:
			FindIntersections(wiresB, activeB, wiresA[wireIndex], &intersections);
			activeA.push_back(wireIndex);
			break;

		case RangeType::OpenB:
			FindIntersections(wiresA, activeA, wiresB[wireIndex], &intersections);
			activeB.push_back(wireIndex);
			break;

		case RangeType::CloseB:
			swap(*ranges::find(activeB, wireIndex), *activeB.rbegin());
			activeB.pop_back();
			break;

		case RangeType::CloseA:
			swap(*ranges::find(activeA, wireIndex), *activeA.rbegin());
			activeA.pop_back();
			break;
		}
	}

	return intersections;
}

void Puzzle03_A_2019()
{
	vector<WireSegment> wiresA = WireSegments();
	vector<WireSegment> wiresB = WireSegments();

	vector<tuple<int32_t, RangeType, int32_t>> ranges = SortedRanges(wiresA, wiresB);
	vector<Intersection> intersections = FindAllIntersections(wiresA, wiresB, ranges);

	MinValue<int32_t> answer;
	for (const Intersection& intersection : intersections)
	{
		if (intersection.Pos != Vec2Int{})
		{
			answer.Update(abs(intersection.Pos.X) + abs(intersection.Pos.Y));
		}
	}

	PuzzleOutput::Submit(2019, 3, 1, answer.Get());
}

void Puzzle03_B_2019()
{
	vector<WireSegment> wiresA = WireSegments();
	vector<WireSegment> wiresB = WireSegments();

	vector<tuple<int32_t, RangeType, int32_t>> ranges = SortedRanges(wiresA, wiresB);
	vector<Intersection> intersections = FindAllIntersections(wiresA, wiresB, ranges);

	MinValue<int32_t> answer;
	for (const Intersection& intersection : intersections)
	{
		if (intersection.CombinedSteps != 0)
		{
			answer.Update(intersection.CombinedSteps);
		}
	}

	PuzzleOutput::Submit(2019, 3, 2, answer.Get());
}

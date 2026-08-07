#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle06_2018_Types
{
	struct CoordinateSpace
	{
		vector<Vec2Int> Coords;
		int32_t Left;
		int32_t Right;
		int32_t Top;
		int32_t Bottom;
	};

	class Bitfield2D
	{
	public:
		Bitfield2D()
			: Storage((512 * 512) >> 5)
		{
		}

		bool GetAndSet(const Vec2Int& pos)
		{
			assert((pos.X & ~0x1ff) == 0);
			assert((pos.Y & ~0x1ff) == 0);

			size_t index = (pos.X >> 5) | (pos.Y << 4);
			uint32_t shift = pos.X & 0x1f;

			uint32_t bits = Storage[index];
			uint32_t bit = (1 << shift);

			bool wasAlreadySet = (bits & bit);
			bits |= bit;
			Storage[index] = bits;

			return wasAlreadySet;
		}

	private:

		vector<uint32_t> Storage;
	};
}

using namespace Puzzle06_2018_Types;

static CoordinateSpace ReadCoordinates()
{
	CoordinateSpace ret;
	ret.Coords.reserve(64);

	ret.Left = numeric_limits<int32_t>::max();
	ret.Right = numeric_limits<int32_t>::min();
	ret.Top = numeric_limits<int32_t>::max();
	ret.Bottom = numeric_limits<int32_t>::min();

	while (PuzzleInput::NextLine())
	{
		Vec2Int c{ Parse::GetInt32(), Parse::GetInt32() };
		ret.Left = min(ret.Left, c.X);
		ret.Right = max(ret.Right, c.X);
		ret.Top = min(ret.Top, c.Y);
		ret.Bottom = max(ret.Bottom, c.Y);
		ret.Coords.push_back(c);
	}

	return ret;
}

static void TagInfiniteRegions(const CoordinateSpace& space, const Vec2Int& edgeFrom, const Vec2Int& edgeTo, vector<bool>* regions)
{
	for (Vec2Int pos : uLineInclusiveRange{ edgeFrom, edgeTo })
	{
		int32_t nearestDistance = numeric_limits<int32_t>::max();
		size_t nearestTo = numeric_limits<size_t>::max();
		int32_t nearestCount = 0;
		for (size_t i = 0; i < space.Coords.size(); i++)
		{
			int32_t distance = ManhattanDistance(pos, space.Coords[i]);
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearestTo = i;
				nearestCount = 1;
			}
			else if (distance == nearestDistance)
			{
				nearestCount++;
			}
		}
		if (nearestCount == 1)
		{
			(*regions)[nearestTo] = false;
		}
	}
}

static vector<size_t> GetFiniteRegions(const CoordinateSpace& space)
{
	vector<bool> isFiniteRegion(space.Coords.size(), true);

	TagInfiniteRegions(space, Vec2Int{ space.Left, space.Top }, Vec2Int{ space.Right, space.Top }, &isFiniteRegion); // Across the top
	TagInfiniteRegions(space, Vec2Int{ space.Left, space.Top }, Vec2Int{ space.Left, space.Bottom }, &isFiniteRegion); // Down left
	TagInfiniteRegions(space, Vec2Int{ space.Right, space.Top }, Vec2Int{ space.Right, space.Bottom }, &isFiniteRegion); // Down right
	TagInfiniteRegions(space, Vec2Int{ space.Left, space.Bottom }, Vec2Int{ space.Right, space.Bottom }, &isFiniteRegion); // Across the bottom

	vector<size_t> finiteRegions;
	finiteRegions.reserve(space.Coords.size());
	for (size_t i = 0; i < space.Coords.size(); i++)
	{
		if (isFiniteRegion[i])
		{
			finiteRegions.push_back(i);
		}
	}

	return finiteRegions;
}

static size_t SizeOfRegion(const vector<Vec2Int>& regionCentres)
{
	vector<Vec2Int> searchQueue;
	searchQueue.reserve(4 * 1024);
	searchQueue.push_back(regionCentres[0]);

	Bitfield2D evaluated;
	evaluated.GetAndSet(regionCentres[0]);

	for (size_t searchIndex = 0; searchIndex < searchQueue.size(); searchIndex++)
	{
		Vec2Int current = searchQueue[searchIndex];
		for (const Vec2Int& dir : Vec2Int::CardinalDirections())
		{
			Vec2Int neighbour = current + dir;
			if (evaluated.GetAndSet(neighbour) == false)
			{
				MinValue<int32_t> minDistanceToOther;
				for (size_t i = 1; i < regionCentres.size(); i++)
				{
					int32_t distanceToOther = ManhattanDistance(regionCentres[i], neighbour);
					minDistanceToOther.Update(distanceToOther);
				}

				int32_t distanceToRegionCentre = ManhattanDistance(regionCentres[0], neighbour);
				if (distanceToRegionCentre < minDistanceToOther)
				{
					searchQueue.push_back(neighbour);
				}
			}
		}
	}

	return searchQueue.size();
}

static size_t LargestFiniteRegion(const CoordinateSpace& space, const vector<size_t>& finiteRegions)
{
	vector<Vec2Int> regionCentres = space.Coords;

	MaxValue<size_t> largestSize;
	for (size_t region : finiteRegions)
	{
		swap(regionCentres[0], regionCentres[region]);
		size_t regionSize = SizeOfRegion(regionCentres);
		largestSize.Update(regionSize);
		swap(regionCentres[0], regionCentres[region]);
	}

	return largestSize;
}

static vector<int32_t> GetDistances(const vector<int32_t>& axis, int32_t rangeBegin, int32_t rangeEnd, int32_t maxDistance)
{
	vector<int32_t> distances;
	distances.reserve(rangeEnd - rangeBegin);

	for (int32_t r = rangeBegin; r < rangeEnd; r++)
	{
		int32_t distance = 0;
		for (size_t i = 0; i < axis.size(); i++)
		{
			distance += abs(axis[i] - r);
		}
		if (distance < maxDistance)
		{
			distances.push_back(distance);
		}
	}

	return distances;
}

void Puzzle06_A_2018()
{
	CoordinateSpace space = ReadCoordinates();
	vector<size_t> finiteRegions = GetFiniteRegions(space);
	size_t answer = LargestFiniteRegion(space, finiteRegions);

	PuzzleOutput::Submit(2018, 6, 1, answer);

}

void Puzzle06_B_2018()
{
	const int32_t maxDistance = 10000;

	vector<int32_t> xAxis;
	xAxis.reserve(64);

	vector<int32_t> yAxis;
	yAxis.reserve(64);

	MinMaxValues<int32_t> xMinMax, yMinMax;
	while (PuzzleInput::NextLine())
	{
		int32_t x = Parse::GetInt32();
		int32_t y = Parse::GetInt32();

		xMinMax.Update(x);
		yMinMax.Update(y);

		xAxis.push_back(x);
		yAxis.push_back(y);
	}

	assert((maxDistance % static_cast<int32_t>(xAxis.size())) == 0);
	const int32_t padding = maxDistance / static_cast<int32_t>(xAxis.size());

	vector<int32_t> xDistances = GetDistances(xAxis, xMinMax.GetMin() - padding, xMinMax.GetMax() + padding + 1, maxDistance);
	vector<int32_t> yDistances = GetDistances(yAxis, yMinMax.GetMin() - padding, yMinMax.GetMax() + padding + 1, maxDistance);

	int32_t answer = 0;
	for (int32_t yDist : yDistances)
	{
		for (int32_t xDist : xDistances)
		{
			if ((xDist + yDist) < maxDistance)
			{
				answer++;
			}
		}
	}

	PuzzleOutput::Submit(2018, 6, 2, answer);
}

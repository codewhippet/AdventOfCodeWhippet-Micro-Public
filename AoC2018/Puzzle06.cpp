#include "stdafx.h"

using namespace std;

namespace Puzzle06_2018_Types
{
}

using namespace Puzzle06_2018_Types;

static void TagInfiniteRegions(const vector<Vec2Int>& coords, const Vec2Int& edgeFrom, const Vec2Int& edgeTo, vector<bool>* regions)
{
	for (Vec2Int pos : uLineInclusiveRange{ edgeFrom, edgeTo })
	{
		int32_t nearestDistance = numeric_limits<int32_t>::max();
		size_t nearestTo = numeric_limits<size_t>::max();
		for (size_t i = 0; i < coords.size(); i++)
		{
			int32_t distance = ManhattanDistance(pos, coords[i]);
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearestTo = i;
			}
			else if (distance == nearestDistance)
			{
				nearestTo = numeric_limits<size_t>::max();
			}
		}
		if (nearestTo != numeric_limits<size_t>::max())
		{
			(*regions)[nearestTo] = false;
		}
	}
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
	vector<Vec2Int> coords;
	coords.reserve(64);

	MinMaxValues<int32_t> xMinMax, yMinMax;
	while (PuzzleInput::NextLine())
	{
		int32_t x = Parse::GetInt32();
		int32_t y = Parse::GetInt32();

		xMinMax.Update(x);
		yMinMax.Update(y);

		coords.push_back({ x, y });
	}

	vector<bool> isFiniteRegion(coords.size(), true);

	TagInfiniteRegions(coords, Vec2Int{ xMinMax.GetMin(), yMinMax.GetMin() }, Vec2Int{ xMinMax.GetMax(), yMinMax.GetMin() }, &isFiniteRegion); // Across the top
	TagInfiniteRegions(coords, Vec2Int{ xMinMax.GetMin(), yMinMax.GetMin() }, Vec2Int{ xMinMax.GetMin(), yMinMax.GetMax() }, &isFiniteRegion); // Down left
	TagInfiniteRegions(coords, Vec2Int{ xMinMax.GetMax(), yMinMax.GetMin() }, Vec2Int{ xMinMax.GetMax(), yMinMax.GetMax() }, &isFiniteRegion); // Down right
	TagInfiniteRegions(coords, Vec2Int{ xMinMax.GetMin(), yMinMax.GetMax() }, Vec2Int{ xMinMax.GetMax(), yMinMax.GetMax() }, &isFiniteRegion); // Across the bottom

	vector<int32_t> regionSizes(coords.size(), 0);

	vector<int32_t> yDistances(coords.size(), -1);
	for (int32_t y = yMinMax.GetMin() + 1; y < yMinMax.GetMax(); y++)
	{
		for (size_t i = 0; i < coords.size(); i++)
		{
			yDistances[i] = abs(coords[i].Y - y);
		}

		for (int32_t x = xMinMax.GetMin() + 1; x < xMinMax.GetMax(); x++)
		{
			int32_t nearestRegionDistance = numeric_limits<int32_t>::max();
			size_t nearestRegion = numeric_limits<size_t>::max();
			for (size_t i = 0; i < coords.size(); i++)
			{
				int32_t regionDistance = abs(coords[i].X - x) + yDistances[i];
				if (regionDistance < nearestRegionDistance)
				{
					nearestRegionDistance = regionDistance;
					nearestRegion = static_cast<int32_t>(i);
				}
				else if (regionDistance == nearestRegionDistance)
				{
					nearestRegion = numeric_limits<size_t>::max();
				}
			}

			if (nearestRegion != numeric_limits<size_t>::max())
			{
				regionSizes[nearestRegion]++;
			}
		}
	}

	MaxValue<int32_t> largestFiniteRegion;
	for (size_t i = 0; i < regionSizes.size(); i++)
	{
		if (isFiniteRegion[i])
		{
			largestFiniteRegion.Update(regionSizes[i]);
		}
	}

	int32_t answer = largestFiniteRegion.Get();
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

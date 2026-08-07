#include "stdafx.h"

using namespace std;

namespace Puzzle20_2016_Types
{
}

using namespace Puzzle20_2016_Types;

static vector<pair<uint32_t, uint32_t>> SortAndMergeRanges(vector<pair<uint32_t, uint32_t>> ranges)
{
	ranges::sort(ranges);

	size_t dest = 0;
	for (size_t src = 1; src < ranges.size(); src++)
	{
		assert(ranges[src].first > 0);
		if ((ranges[src].first - 1) <= ranges[dest].second)
		{
			ranges[dest].second = max(ranges[dest].second, ranges[src].second);
		}
		else
		{
			dest++;
			ranges[dest] = ranges[src];
		}
	}

	ranges.resize(dest + 1);

	return ranges;
}

void Puzzle20_A_2016()
{
	vector<pair<uint32_t, uint32_t>> ranges;
	ranges.reserve(1024);
	while (PuzzleInput::NextLine())
	{
		uint32_t start = Parse::GetUint32();
		uint32_t stop = Parse::GetUint32();
		ranges.push_back({ start, stop });
	}

	ranges = SortAndMergeRanges(ranges);

	int32_t answer = ranges.front().second + 1;
	return PuzzleOutput::Submit(2016, 20, 1, answer);
}

void Puzzle20_B_2016()
{
	vector<pair<uint32_t, uint32_t>> ranges;
	ranges.reserve(1024);
	while (PuzzleInput::NextLine())
	{
		uint32_t start = Parse::GetUint32();
		uint32_t stop = Parse::GetUint32();
		ranges.push_back({ start, stop });
	}

	ranges = SortAndMergeRanges(ranges);

	int32_t answer = ranges.front().first;
	for (size_t i = 0; (i + 1) < ranges.size(); i++)
	{
		answer += ranges[i + 1].first - ranges[i].second - 1;
	}
	answer += numeric_limits<uint32_t>::max() - ranges.back().second;

	return PuzzleOutput::Submit(2016, 20, 2, answer);
}

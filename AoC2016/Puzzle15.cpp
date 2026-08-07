#include "stdafx.h"

using namespace std;

namespace Puzzle15_2016_Types
{
}

using namespace Puzzle15_2016_Types;

static vector<pair<int32_t, int32_t>> ParseDiscs()
{
	vector<pair<int32_t, int32_t>> discs;
	discs.reserve(16);

	while (PuzzleInput::NextLine())
	{
		Parse::GetInt32();
		int32_t numPositions = Parse::GetInt32();
		Parse::GetInt32();
		int32_t atPosition = Parse::GetInt32();
		PuzzleInput::DropLine();

		discs.push_back({ numPositions, atPosition });
	}

	return discs;
}

static vector<int32_t> CalculateSyncPoint(const vector<pair<int32_t, int32_t>>& discs)
{
	vector<int32_t> needToBeAt;
	for (int32_t discIndex = 0; discIndex < (int32_t)discs.size(); discIndex++)
	{
		const pair<int32_t, int32_t>& disc = discs[discIndex];
		int32_t needsToBe = (disc.first - ((discIndex + 1) % disc.first)) % disc.first;
		needToBeAt.push_back(needsToBe);
	}
	return needToBeAt;
}

static int32_t FindTimeForAlignment(const vector<pair<int32_t, int32_t>>& discs, const vector<int32_t>& needToBeAt)
{
	assert(discs.size() == needToBeAt.size());

	int32_t step = 1;
	int32_t time = 0;
	for (size_t discIndex = 0; discIndex < discs.size(); discIndex++)
	{
		const pair<int32_t, int32_t>& disc = discs[discIndex];
		while (((disc.second + time) % disc.first) != needToBeAt[discIndex])
		{
			time += step;
		}
		step *= disc.first;
	}

	return time;
}

void Puzzle15_A_2016()
{
	vector<pair<int32_t, int32_t>> discs = ParseDiscs();
	vector<int32_t> needToBeAt = CalculateSyncPoint(discs);

	int32_t answer = FindTimeForAlignment(discs, needToBeAt);

	return PuzzleOutput::Submit(2016, 15, 1, answer);
}

void Puzzle15_B_2016()
{
	vector<pair<int32_t, int32_t>> discs = ParseDiscs();
	discs.push_back({ 11, 0 });
	vector<int32_t> needToBeAt = CalculateSyncPoint(discs);

	int32_t answer = FindTimeForAlignment(discs, needToBeAt);

	return PuzzleOutput::Submit(2016, 15, 2, answer);
}

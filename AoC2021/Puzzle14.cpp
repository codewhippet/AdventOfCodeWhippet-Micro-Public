#include "stdafx.h"

using namespace std;

namespace Puzzle14_2021_Types
{
	struct Mapping
	{
		uint32_t From;
		pair<uint32_t, uint32_t> To;
	};
}

using namespace Puzzle14_2021_Types;

static uint32_t PairToIndex(char a, char b)
{
	return ((a - 'A') << 5) | (b - 'A');
}

static vector<Mapping> CreateTransformMapping()
{
	const size_t mappingReserveSize = 100;

	vector<Mapping> mapping;
	mapping.reserve(mappingReserveSize);

	array<char, 8> line;
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line.data(), line.size());

		Mapping m;
		m.From = PairToIndex(line[0], line[1]);
		m.To.first = PairToIndex(line[0], line[6]);
		m.To.second = PairToIndex(line[6], line[1]);
		mapping.push_back(m);
	}

	return mapping;
}

static int64_t Solve(size_t rounds)
{
	const size_t alphabetSize = 32;
	const size_t polymerReserveSize = 32;

	array<char, polymerReserveSize> startingPolymer;
	int32_t startingPolymerSize = Parse::ReadNonEmptyLine(startingPolymer.data(), startingPolymer.size());
	PuzzleInput::NextLine();

	vector<Mapping> mappings = CreateTransformMapping();

	array<vector<int64_t>, 2> dp;
	for (size_t i = 0; i < dp.size(); i++)
	{
		dp[i].resize(alphabetSize * alphabetSize);
	};

	for (size_t i = 0; (i + 1) < startingPolymerSize; i++)
	{
		dp[0][PairToIndex(startingPolymer[i], startingPolymer[i + 1])]++;
	}

	for (size_t expansion = 0; expansion < rounds; expansion++)
	{
		const vector<int64_t>& current = dp[expansion & 1];
		vector<int64_t>& next = dp[(expansion + 1) & 1];

		ranges::fill(next, 0);
		for (const Mapping& m : mappings)
		{
			int64_t pairCount = current[m.From];
			if (pairCount > 0)
			{
				next[m.To.first] += pairCount;
				next[m.To.second] += pairCount;
			}
		}
	}

	vector<int64_t> polymerCounts(alphabetSize);
	for (size_t i = 0; i < alphabetSize; i++)
	{
		for (size_t j = 0; j < alphabetSize; j++)
		{
			int64_t pairCount = dp[rounds & 1][(i << 5) | j];
			polymerCounts[i] += pairCount;
			polymerCounts[j] += pairCount;
		}
	}

	polymerCounts[startingPolymer[0] - 'A']++;
	polymerCounts[startingPolymer[startingPolymerSize - 1] - 'A']++;

	MinMaxValues<int64_t> polymers;
	for (size_t i = 0; i < polymerCounts.size(); i++)
	{
		if (polymerCounts[i] > 0)
		{
			polymers.Update(polymerCounts[i] >> 1);
		}
	}

	return polymers.GetMax() - polymers.GetMin();
}

void Puzzle14_A_2021()
{
	int64_t answer = Solve(10);

	PuzzleOutput::Submit(2021, 14, 1, answer);
}

void Puzzle14_B_2021()
{
	int64_t answer = Solve(40);

	PuzzleOutput::Submit(2021, 14, 2, answer);
}

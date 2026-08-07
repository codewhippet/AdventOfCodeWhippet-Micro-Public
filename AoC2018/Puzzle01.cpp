#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle01_2018_Types
{
}

using namespace Puzzle01_2018_Types;

void Puzzle01_A_2018()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		answer += Parse::GetInt32();
	}

	PuzzleOutput::Submit(2018, 1, 1, answer);
}

void Puzzle01_B_2018()
{
	vector<int32_t> baseFrequencies;
	baseFrequencies.reserve(1024);

	// We make the simplifying assumption that frequency 0 is not the answer, and that
	// the duplicate frequency doesn't occur in the first run through
	int32_t loopDiff = 0;
	while (PuzzleInput::NextLine())
	{
		loopDiff += Parse::GetInt32();
		baseFrequencies.push_back(loopDiff);
	}
	assert(loopDiff > 0);

	// Distribute frequencies into potentially colliding buckets
	vector<array<size_t, 3>> buckets(loopDiff,
		{
			numeric_limits<size_t>::max(),
			numeric_limits<size_t>::max(),
			numeric_limits<size_t>::max()
		});
	vector<size_t> bucketCounts(loopDiff);

	for (size_t freqIndex = 0; freqIndex < baseFrequencies.size(); freqIndex++)
	{
		int32_t freq = baseFrequencies[freqIndex];
		int32_t freqMod = freq % loopDiff;
		if (freqMod < 0)
		{
			freqMod += loopDiff;
		}

		assert(bucketCounts[freqMod] < 3);
		buckets[freqMod][bucketCounts[freqMod]++] = freqIndex;
	}

	// Find the first collision
	int32_t earliestCollidingLoop = numeric_limits<int32_t>::max();
	size_t earliestCollidingIndex = numeric_limits<size_t>::max();
	for (size_t bucketIndex = 0; bucketIndex < buckets.size(); bucketIndex++)
	{
		const array<size_t, 3>& bucket = buckets[bucketIndex];
		size_t bucketSize = bucketCounts[bucketIndex];

		for (size_t i = 0; (i + 1) < bucketSize; i++)
		{
			for (size_t j = i + 1; j < bucketSize; j++)
			{
				size_t indexA = bucket[i];
				size_t indexB = bucket[j];

				int32_t freqA = baseFrequencies[indexA];
				int32_t freqB = baseFrequencies[indexB];

				int32_t freqDiff = abs(freqB - freqA);
				int32_t loopsUntilCollision = freqDiff / loopDiff;
				if (loopsUntilCollision <= earliestCollidingLoop)
				{
					earliestCollidingLoop = loopsUntilCollision;
					earliestCollidingIndex = min(earliestCollidingIndex, indexA); // indexA will always be less than indexB
				}
			}
		}
	}
	
	int32_t answer = (earliestCollidingLoop * loopDiff) + baseFrequencies[earliestCollidingIndex];

	PuzzleOutput::Submit(2018, 1, 2, answer);
}

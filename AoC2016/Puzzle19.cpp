#include "stdafx.h"

using namespace std;

namespace Puzzle19_2016_Types
{
}

using namespace Puzzle19_2016_Types;

void Puzzle19_A_2016()
{
	uint32_t elfCount = Parse::GetInt32();

	// Classic Josephus problem with k=2
	// https://en.wikipedia.org/wiki/Josephus_problem#k_=_2
	int32_t largestPowerOfTwo = 1 << (31 - countl_zero(elfCount));
	int32_t countAfterPowerOfTwo = elfCount - largestPowerOfTwo;
	int32_t answer = (countAfterPowerOfTwo * 2) + 1;

	return PuzzleOutput::Submit(2016, 19, 1, answer);
}

void Puzzle19_B_2016()
{
	double elfCount = Parse::GetInt32();

	// Formula worked out in Excel after staring at a graph
	const double log3 = log(3);

	int32_t answer = -1;

	double largestPowerOfThreeLog = floor(log(elfCount) / log3);
	double largestPowerOfThree = pow(3.0, largestPowerOfThreeLog);
	if (elfCount == largestPowerOfThree)
	{
		answer = static_cast<int32_t>(elfCount);
	}
	else
	{
		double singleIncrement = elfCount - largestPowerOfThree;
		double doubleIncrement = max(singleIncrement - largestPowerOfThree, 0.0);
		answer = static_cast<int32_t>(singleIncrement + doubleIncrement);
	}

	// Note: looking up afterwards it's actually https://oeis.org/A334473

	return PuzzleOutput::Submit(2016, 19, 2, answer);
}

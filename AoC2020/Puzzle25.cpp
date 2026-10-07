#include "stdafx.h"

using namespace std;

namespace Puzzle25_2020_Types
{
}

using namespace Puzzle25_2020_Types;

static uint32_t FindLoopSize(uint32_t subjectNumber, uint32_t targetNumber, uint32_t mod)
{
	uint64_t workingValue = 1;
	for (uint32_t loopSize = 0; loopSize < mod; loopSize++)
	{
		workingValue = (workingValue * subjectNumber) % mod;
		if (workingValue == targetNumber)
		{
			return loopSize + 1;
		}
	}

	return 0;
}

static uint32_t PowMod(uint64_t base, uint32_t exp, uint32_t mod)
{
	uint64_t result = 1;
	base = base % mod;
	while (exp)
	{
		if (exp & 1)
		{
			result = (result * base) % mod;
		}
		exp >>= 1;
		base = (base * base) % mod;
	}
	return static_cast<uint32_t>(result);
}

void Puzzle25_A_2020()
{
	const uint32_t cardPublicKey = Parse::GetUint32();
	const uint32_t doorPublicKey = Parse::GetUint32();

	const uint32_t cardLoopSize = FindLoopSize(7, cardPublicKey, 20201227);
	const uint32_t cardTransform = PowMod(doorPublicKey, cardLoopSize, 20201227);

	int32_t answer = cardTransform;
	PuzzleOutput::Submit(2020, 25, 1, answer);
}

void Puzzle25_B_2020()
{
	return PuzzleOutput::Submit(2020, 25, 2, int64_t(-1));
}

#include "stdafx.h"

using namespace std;

namespace Puzzle04_2022_Types
{
}

using namespace Puzzle04_2022_Types;

void Puzzle04_A_2022()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		uint32_t firstBegin = Parse::GetUint32();
		uint32_t firstEnd = Parse::GetUint32();

		uint32_t secondBegin = Parse::GetUint32();
		uint32_t secondEnd = Parse::GetUint32();

		bool firstContainsSecond = (firstBegin <= secondBegin) && (firstEnd >= secondEnd);
		bool secondContainsFirst = (secondBegin <= firstBegin) && (secondEnd >= firstEnd);

		if (firstContainsSecond || secondContainsFirst)
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2022, 4, 1, answer);
}

void Puzzle04_B_2022()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		uint32_t firstBegin = Parse::GetUint32();
		uint32_t firstEnd = Parse::GetUint32();

		uint32_t secondBegin = Parse::GetUint32();
		uint32_t secondEnd = Parse::GetUint32();

		bool firstLess = firstEnd < secondBegin;
		bool secondLess = secondEnd < firstBegin;
		bool isDisjoint = firstLess || secondLess;

		if (isDisjoint == false)
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2022, 4, 2, answer);
}

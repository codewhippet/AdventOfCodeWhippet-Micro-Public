#include "stdafx.h"

using namespace std;

namespace Puzzle02_2021_Types
{
}

using namespace Puzzle02_2021_Types;

void Puzzle02_A_2021()
{
	int32_t horizontal = 0;
	int32_t depth = 0;

	while (PuzzleInput::NextLine())
	{
		int inst = PuzzleInput::GetChar();
		int32_t amount = Parse::GetInt32();

		switch (inst)
		{
		case 'f':
			horizontal += amount;
			break;
		case 'u':
			depth -= amount;
			break;
		case 'd':
			depth += amount;
			break;
		}
	}

	int32_t answer = horizontal * depth;

	PuzzleOutput::Submit(2021, 2, 1, answer);
}

void Puzzle02_B_2021()
{
	int32_t horizontal = 0;
	int32_t depth = 0;
	int32_t aim = 0;

	while (PuzzleInput::NextLine())
	{
		int inst = PuzzleInput::GetChar();
		int32_t amount = Parse::GetInt32();

		switch (inst)
		{
		case 'f':
			horizontal += amount;
			depth += aim * amount;
			break;
		case 'u':
			aim -= amount;
			break;
		case 'd':
			aim += amount;
			break;
		}
	}

	int32_t answer = horizontal * depth;

	PuzzleOutput::Submit(2021, 2, 2, answer);
}

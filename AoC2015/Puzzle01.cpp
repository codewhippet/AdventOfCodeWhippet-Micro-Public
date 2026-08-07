#include "stdafx.h"

using namespace std;

namespace Puzzle01_2015_Types
{
}

using namespace Puzzle01_2015_Types;

void Puzzle01_A_2015()
{
	int32_t answer = 0;
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		answer += (c == '(') ? 1 : -1;
	}
	return PuzzleOutput::Submit(2015, 1, 1, answer);
}

void Puzzle01_B_2015()
{
	int32_t instructionCount = 0;
	int32_t currentFloor = 0;
	for (int32_t c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		currentFloor += (c == '(') ? 1 : -1;
		if (currentFloor < 0)
		{
			return PuzzleOutput::Submit(2015, 1, 2, instructionCount + 1);
		}

		instructionCount++;
	}
	return PuzzleOutput::Submit(2015, 1, 2, int32_t(-1));
}

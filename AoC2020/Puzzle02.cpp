#include "stdafx.h"

using namespace std;

namespace Puzzle02_2020_Types
{
}

using namespace Puzzle02_2020_Types;

void Puzzle02_A_2020()
{
	int32_t answer = 0;

	char line[32];
	while (PuzzleInput::NextLine())
	{
		int32_t minRepeats = Parse::GetUint32();
		int32_t maxRepeats = Parse::GetUint32();
		PuzzleInput::DropChar();
		int needle = PuzzleInput::GetChar();

		PuzzleInput::DropChar();
		PuzzleInput::DropChar();

		Parse::ReadNonEmptyLine(line);

		int32_t numRepeats = 0;
		for (const char* c = &line[0]; *c; c++)
		{
			numRepeats += (*c == needle);
		}

		answer += (numRepeats >= minRepeats) && (numRepeats <= maxRepeats);
	}

	PuzzleOutput::Submit(2020, 2, 1, answer);
}

void Puzzle02_B_2020()
{
	int32_t answer = 0;

	char line[32];
	while (PuzzleInput::NextLine())
	{
		int32_t posOne = Parse::GetUint32() - 1;
		int32_t posTwo = Parse::GetUint32() - 1;
		PuzzleInput::DropChar();
		int needle = PuzzleInput::GetChar();

		PuzzleInput::DropChar();
		PuzzleInput::DropChar();

		Parse::ReadNonEmptyLine(line);

		answer += (line[posOne] == needle) ^ (line[posTwo] == needle);
	}

	PuzzleOutput::Submit(2020, 2, 2, answer);
}

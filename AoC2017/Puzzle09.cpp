#include "stdafx.h"

using namespace std;

namespace Puzzle09_2017_Types
{
}

using namespace Puzzle09_2017_Types;

static int32_t IgnoreGarbage()
{
	int32_t ignored = 0;

	for (int c = PuzzleInput::GetChar(); c != '>'; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case '!':
			PuzzleInput::DropChar();
			break;
		default:
			ignored++;
		}
	}

	return ignored;
}

static int32_t ScoreGroups()
{
	int32_t score = 0;

	int32_t indentationLevel = 0;
	for (int c = PuzzleInput::GetChar(); c != EOF && c != '\n'; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case '{':
			score += ++indentationLevel;
			break;
		case '}':
			indentationLevel--;
			break;
		case '<':
			IgnoreGarbage();
			break;
		case ',':
			break;
		default:
			assert(false);
		}
	}

	return score;
}

static int32_t AllIgnoredGarbage()
{
	int32_t ignored = 0;

	for (int c = PuzzleInput::GetChar(); c != EOF && c != '\n'; c = PuzzleInput::GetChar())
	{
		if (c == '<')
		{
			ignored += IgnoreGarbage();
		}
	}

	return ignored;
}

void Puzzle09_A_2017()
{
	int32_t answer = ScoreGroups();

	PuzzleOutput::Submit(2017, 9, 1, answer);
}

void Puzzle09_B_2017()
{
	int32_t answer = AllIgnoredGarbage();

	PuzzleOutput::Submit(2017, 9, 2, answer);
}

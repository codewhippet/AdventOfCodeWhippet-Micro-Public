#include "stdafx.h"

using namespace std;

namespace Puzzle02_2016_Types
{
}

using namespace Puzzle02_2016_Types;

static string GetKeyCode(const map<Vec2Int, char>& keypad, const Vec2Int& startingLocation)
{
	const map<char, Vec2Int> directions =
	{
		{ 'U', Vec2Int::Up() },
		{ 'D', Vec2Int::Down() },
		{ 'L', Vec2Int::Left() },
		{ 'R', Vec2Int::Right() },
	};

	Vec2Int fingerLocation = startingLocation;

	string code;
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case 'U':
		case 'D':
		case 'L':
		case 'R':
			{
			Vec2Int newFingerLocation = fingerLocation + directions.at((char)c);
				if (keypad.contains(newFingerLocation))
				{
					fingerLocation = newFingerLocation;
				}
			}
			break;

		case '\n':
			code += keypad.at(fingerLocation);
			break;

		default:
			assert(false);
		}		
	}
	return code;
}

void Puzzle02_A_2016()
{
	const map<Vec2Int, char> keypad =
	{
		{ Vec2Int::Up() + Vec2Int::Left(), '1' },
		{ Vec2Int::Up(), '2' },
		{ Vec2Int::Up() + Vec2Int::Right(), '3' },

		{ Vec2Int::Left(), '4' },
		{ Vec2Int(), '5' },
		{ Vec2Int::Right(), '6' },

		{ Vec2Int::Down() + Vec2Int::Left(), '7' },
		{ Vec2Int::Down(), '8' },
		{ Vec2Int::Down() + Vec2Int::Right(), '9' },
	};

	string answer = GetKeyCode(keypad, {});
	PuzzleOutput::Submit(2016, 2, 1, answer.c_str());
}

void Puzzle02_B_2016()
{
	const map<Vec2Int, char> keypad =
	{
		{ Vec2Int::Up() + Vec2Int::Up(), '1' },

		{ Vec2Int::Up() + Vec2Int::Left(), '2' },
		{ Vec2Int::Up(), '3' },
		{ Vec2Int::Up() + Vec2Int::Right(), '4' },

		{ Vec2Int::Left() + Vec2Int::Left(), '5' },
		{ Vec2Int::Left(), '6' },
		{ Vec2Int(), '7' },
		{ Vec2Int::Right(), '8' },
		{ Vec2Int::Right() + Vec2Int::Right(), '9' },

		{ Vec2Int::Down() + Vec2Int::Left(), 'A' },
		{ Vec2Int::Down(), 'B' },
		{ Vec2Int::Down() + Vec2Int::Right(), 'C' },

		{ Vec2Int::Down() + Vec2Int::Down(), 'D' },
	};

	string answer = GetKeyCode(keypad, {});
	PuzzleOutput::Submit(2016, 2, 2, answer.c_str());
}

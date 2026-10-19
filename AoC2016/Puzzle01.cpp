#include "stdafx.h"

using namespace std;

namespace Puzzle01_2016_Types
{
}

using namespace Puzzle01_2016_Types;

void Puzzle01_A_2016()
{
	Vec2Int location{ 0, 0 };
	Vec2Int direction = Vec2Int::North();

	for (int directionCode = PuzzleInput::GetChar(); directionCode != EOF; directionCode = PuzzleInput::GetChar())
	{
		int32_t length = Parse::GetInt32();
		if (directionCode == 'L')
		{
			direction = Vec2Int::RotateAnticlockwise(direction);
		}
		else if (directionCode == 'R')
		{
			direction = Vec2Int::RotateClockwise(direction);
		}
		else
		{
			assert(false);
		}

		location = location + direction * length;

		PuzzleInput::DropChar();
		PuzzleInput::DropChar();
	}

	int32_t answer = abs(location.X) + abs(location.Y);

	return PuzzleOutput::Submit(2016, 1, 1, answer);
}


void Puzzle01_B_2016()
{
	Vec2Int location{ 0, 0 };
	Vec2Int direction = Vec2Int::North();

	HashSet<Vec2Int> visited(2 * 1024, Vec2Int::Max());
	visited.Insert(location);

	for (int directionCode = PuzzleInput::GetChar(); directionCode != EOF; directionCode = PuzzleInput::GetChar())
	{
		int32_t length = Parse::GetInt32();
		if (directionCode == 'L')
		{
			direction = Vec2Int::RotateAnticlockwise(direction);
		}
		else if (directionCode == 'R')
		{
			direction = Vec2Int::RotateClockwise(direction);
		}
		else
		{
			assert(false);
		}

		bool foundHq = false;
		for (int32_t i = 0; i < length; i++)
		{
			location = location + direction;
			if (visited.Insert(location) == false)
			{
				foundHq = true;
				break;
			}
		}

		if (foundHq)
			break;

		PuzzleInput::DropChar();
		PuzzleInput::DropChar();
	}

	int32_t answer = abs(location.X) + abs(location.Y);

	return PuzzleOutput::Submit(2016, 1, 2, answer);
}

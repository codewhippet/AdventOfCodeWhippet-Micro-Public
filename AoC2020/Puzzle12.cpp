#include "stdafx.h"

using namespace std;

namespace Puzzle12_2020_Types
{
}

using namespace Puzzle12_2020_Types;

void Puzzle12_A_2020()
{
	Vec2Int position{};
	Vec2Int heading = Vec2Int::East();

	while (PuzzleInput::NextLine())
	{
		int op = PuzzleInput::GetChar();
		int32_t amount = Parse::GetInt32();
		switch (op)
		{
		case 'N':
			position += amount * Vec2Int::North();
			break;
		case 'S':
			position += amount * Vec2Int::South();
			break;
		case 'E':
			position += amount * Vec2Int::East();
			break;
		case 'W':
			position += amount * Vec2Int::West();
			break;

		case 'L':
			switch (amount)
			{
			case 90:
				heading = Vec2Int::RotateAnticlockwise(heading);
				break;
			case 180:
				heading = -heading;
				break;
			case 270:
				heading = Vec2Int::RotateClockwise(heading);
				break;
			}
			break;

		case 'R':
			switch (amount)
			{
			case 90:
				heading = Vec2Int::RotateClockwise(heading);
				break;
			case 180:
				heading = -heading;
				break;
			case 270:
				heading = Vec2Int::RotateAnticlockwise(heading);
				break;
			}
			break;

		case 'F':
			position = position + amount * heading;
			break;
		}
	}

	int32_t answer = ManhattanDistance({}, position);

	PuzzleOutput::Submit(2020, 12, 1, answer);
}

void Puzzle12_B_2020()
{
	Vec2Int position{};
	Vec2Int waypoint = (10 * Vec2Int::East()) + (1 * Vec2Int::North());

	while (PuzzleInput::NextLine())
	{
		int op = PuzzleInput::GetChar();
		int32_t amount = Parse::GetInt32();
		switch (op)
		{
		case 'N':
			waypoint += amount * Vec2Int::North();
			break;
		case 'S':
			waypoint += amount * Vec2Int::South();
			break;
		case 'E':
			waypoint += amount * Vec2Int::East();
			break;
		case 'W':
			waypoint += amount * Vec2Int::West();
			break;

		case 'L':
			switch (amount)
			{
			case 90:
				waypoint = Vec2Int::RotateAnticlockwise(waypoint);
				break;
			case 180:
				waypoint = -waypoint;
				break;
			case 270:
				waypoint = Vec2Int::RotateClockwise(waypoint);
				break;
			}
			break;

		case 'R':
			switch (amount)
			{
			case 90:
				waypoint = Vec2Int::RotateClockwise(waypoint);
				break;
			case 180:
				waypoint = -waypoint;
				break;
			case 270:
				waypoint = Vec2Int::RotateAnticlockwise(waypoint);
				break;
			}
			break;

		case 'F':
			position = position + amount * waypoint;
			break;
		}
	}

	int32_t answer = ManhattanDistance({}, position);

	PuzzleOutput::Submit(2020, 12, 2, answer);
}

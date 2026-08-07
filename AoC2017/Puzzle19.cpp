#include "stdafx.h"

using namespace std;

namespace Puzzle19_2017_Types
{
}

using namespace Puzzle19_2017_Types;

void Puzzle19_A_2017()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 40 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D grid = ReaduArrayMap(' ');

		// Find the starting location
		Vec2Int robotPos;
		Vec2Int robotDir = Vec2Int::South();
		robotPos.Y = grid.GetOrigin().Y;
		for (int32_t x : grid.AxisRangeX())
		{
			if (grid(x, robotPos.Y) == '|')
			{
				robotPos.X = x;
				break;
			}
		}

		// Follow the line
		string answer;
		answer.reserve(32);

		while (true)
		{
			robotPos = robotPos + robotDir;
			char currentSquare = grid(robotPos);

			if (isalpha(currentSquare))
			{
				answer += currentSquare;
			}
			else if (currentSquare == '+')
			{
				char turnRightSquare = grid(robotPos + Vec2Int::RotateClockwise(robotDir));
				char turnLeftSquare = grid(robotPos + Vec2Int::RotateAnticlockwise(robotDir));
				assert(((turnRightSquare == ' ') && (turnLeftSquare != ' '))
					|| ((turnRightSquare != ' ') && (turnLeftSquare == ' ')));
				(void)turnLeftSquare;
				if (turnRightSquare != ' ')
				{
					robotDir = Vec2Int::RotateClockwise(robotDir);
				}
				else
				{
					robotDir = Vec2Int::RotateAnticlockwise(robotDir);
				}
			}
			else if (currentSquare == ' ')
			{
				break;
			}
		}

		PuzzleOutput::Submit(2017, 19, 1, answer.c_str());
	}
	MemArena_Reset();
}

void Puzzle19_B_2017()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 40 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D grid = ReaduArrayMap(' ');

		// Find the starting location
		Vec2Int robotPos;
		Vec2Int robotDir = Vec2Int::South();
		robotPos.Y = grid.GetOrigin().Y;
		for (int32_t x : grid.AxisRangeX())
		{
			if (grid(x, robotPos.Y) == '|')
			{
				robotPos.X = x;
				break;
			}
		}

		// Follow the line
		int32_t answer = 1;
		while (true)
		{
			robotPos = robotPos + robotDir;
			char currentSquare = grid(robotPos);
			answer++;

			if (currentSquare == '+')
			{
				char turnRightSquare = grid(robotPos + Vec2Int::RotateClockwise(robotDir));
				char turnLeftSquare = grid(robotPos + Vec2Int::RotateAnticlockwise(robotDir));
				assert(((turnRightSquare == ' ') && (turnLeftSquare != ' '))
					|| ((turnRightSquare != ' ') && (turnLeftSquare == ' ')));
				(void)turnLeftSquare;
				if (turnRightSquare != ' ')
				{
					robotDir = Vec2Int::RotateClockwise(robotDir);
				}
				else
				{
					robotDir = Vec2Int::RotateAnticlockwise(robotDir);
				}
			}
			else if (currentSquare == ' ')
			{
				answer--;
				break;
			}
		}
		PuzzleOutput::Submit(2017, 19, 2, answer);
	}
	MemArena_Reset();
}

#include "stdafx.h"

using namespace std;

namespace Puzzle25_2021_Types
{
}

using namespace Puzzle25_2021_Types;

static int32_t StepEast(const uArrayMap2D& oldState, uArrayMap2D* newState)
{
	ranges::fill(newState->GetData(), '.');

	int32_t moves = 0;
	for (int32_t y = 0; y < oldState.GetHeight(); y++)
	{
		for (int32_t x = 0; x < oldState.GetWidth(); x++)
		{
			if (oldState(x, y) == '>')
			{
				int32_t newX = (x + 1) % oldState.GetWidth();
				if (oldState(newX, y) == '.')
				{
					(*newState)(newX, y) = '>';
					moves++;
				}
				else
				{
					(*newState)(x, y) = '>';
				}
			}
			else if (oldState(x, y) == 'v')
			{
				(*newState)(x, y) = 'v';
			}
		}
	}

	return moves;
}

static int32_t StepSouth(const uArrayMap2D& oldState, uArrayMap2D* newState)
{
	ranges::fill(newState->GetData(), '.');

	int32_t moves = 0;
	for (int32_t x = 0; x < oldState.GetWidth(); x++)
	{
		for (int32_t y = 0; y < oldState.GetHeight(); y++)
		{
			if (oldState(x, y) == 'v')
			{
				int32_t newY = (y + 1) % oldState.GetHeight();
				if (oldState(x, newY) == '.')
				{
					(*newState)(x, newY) = 'v';
					moves++;
				}
				else
				{
					(*newState)(x, y) = 'v';
				}
			}
			else if (oldState(x, y) == '>')
			{
				(*newState)(x, y) = '>';
			}
		}
	}

	return moves;
}

void Puzzle25_A_2021()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 40 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D buffer0 = ReaduArrayMap();
		uArrayMap2D buffer1(uArrayMap2DOptions::CloneAsUninitialised, buffer0);
		array<uArrayMap2D*, 2> buffers{ &buffer0, &buffer1 };

		int32_t steps = 1;
		while (true)
		{
			int32_t moved = 0;
			moved += StepEast(*buffers[0], buffers[1]);
			moved += StepSouth(*buffers[1], buffers[0]);
			if (moved == 0)
			{
				break;
			}

			steps++;
		}

		int32_t answer = steps;
		PuzzleOutput::Submit(2021, 25, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle25_B_2021()
{
	return PuzzleOutput::Submit(2021, 25, 2, int64_t(-1));
}

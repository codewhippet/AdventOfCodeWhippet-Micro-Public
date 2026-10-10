#include "stdafx.h"

using namespace std;

namespace Puzzle11_2021_Types
{
}

using namespace Puzzle11_2021_Types;

static int32_t Update(uArrayMap2D* board, vector<Vec2Int>* flashQueue, vector<int32_t>* queuedBuffer)
{
	flashQueue->clear();
	ranges::fill(*queuedBuffer, 0);

	ArrayWrapper2D<int32_t> queued(queuedBuffer->data(), board->GetWidth(), board->GetHeight());

	// Increase flash charge
	for (const auto& p : board->Grid())
	{
		if (++(*board)(p.first) > '9')
		{
			flashQueue->push_back(p.first);
			queued(p.first) = 1;
		}
	}

	// Flash and keep flashing
	for (size_t i = 0; i < flashQueue->size(); i++)
	{
		const Vec2Int current = (*flashQueue)[i];
		for (const Vec2Int& dir : Vec2Int::CardinalAndDiagonalDirections())
		{
			const Vec2Int neighbour = current + dir;
			if (!board->IsInside(neighbour))
				continue;

			char incremented = ++(*board)(neighbour);
			if ((incremented > '9') && (queued(neighbour) == 0))
			{
				flashQueue->push_back(neighbour);
				queued(neighbour) = 1;
			}
		}
	}

	// Reset octos
	for (size_t i = 0; i < flashQueue->size(); i++)
	{
		(*board)((*flashQueue)[i]) = '0';
	}

	return static_cast<int32_t>(flashQueue->size());
}

void Puzzle11_A_2021()
{
	const int32_t gridSize = 10;
	const size_t flashQueueSize = gridSize * gridSize;
	const size_t steps = 100;

	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 1 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D board = ReaduArrayMap('0');

		vector<Vec2Int> flashQueue(flashQueueSize);
		vector<int32_t> queuedBuffer(gridSize * gridSize);

		int32_t answer = 0;
		for (size_t i = 0; i < steps; i++)
		{
			int32_t flashed = Update(&board, &flashQueue, &queuedBuffer);
			answer += flashed;
		}

		PuzzleOutput::Submit(2021, 11, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle11_B_2021()
{
	const int32_t gridSize = 10;
	const size_t flashQueueSize = gridSize * gridSize;

	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 1 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D board = ReaduArrayMap('0');

		vector<Vec2Int> flashQueue(flashQueueSize);
		vector<int32_t> queuedBuffer(gridSize * gridSize);

		int32_t answer = 0;
		while (true)
		{
			int32_t flashed = Update(&board, &flashQueue, &queuedBuffer);
			answer++;

			if (flashed == (gridSize * gridSize))
				break;
		}

		PuzzleOutput::Submit(2021, 11, 2, answer);
	}
	MemArena_Reset();
}

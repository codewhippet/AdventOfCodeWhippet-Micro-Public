#include "stdafx.h"

using namespace std;

namespace Puzzle08_2016_Types
{
}

using namespace Puzzle08_2016_Types;

static int32_t UpdateBuffers(array<uArrayMap2D, 2>* displayBuffers)
{
	int32_t currentBufferIndex = 0;
	
	char line[32];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line, sizeof(line));
		string_view instruction{ line };

		currentBufferIndex = 1 - currentBufferIndex;

		uArrayMap2D& currentBuffer = (*displayBuffers)[currentBufferIndex];
		const uArrayMap2D& prevBuffer = (*displayBuffers)[1 - currentBufferIndex];

		currentBuffer = prevBuffer;

		if (instruction.starts_with("rect"sv))
		{
			int32_t x = 0;
			int32_t y = 0;
			int scanned = sscanf(instruction.data(), "rect %dx%d", &x, &y);
			assert(scanned == 2);
			(void)scanned;

			for (const Vec2Int& p : uGridRange(Vec2Int{}, Vec2Int{ x, y }))
			{
				currentBuffer(p) = '#';
			}
		}
		else if (instruction.starts_with("rotate row"sv))
		{
			int32_t index = 0;
			int32_t amount = 0;
			int scanned = sscanf(instruction.data(), "rotate %*s %*c=%d by %d",
				&index,
				&amount);
			assert(scanned == 2);
			(void)scanned;

			assert(amount > -1);

			for (int32_t x = 0; x < currentBuffer.GetWidth(); x++)
			{
				int32_t rotatedX = (x + amount) % currentBuffer.GetWidth();
				currentBuffer(rotatedX, index) = prevBuffer(x, index);
			}
		}
		else if (instruction.starts_with("rotate column"sv))
		{
			int32_t index = 0;
			int32_t amount = 0;
			int scanned = sscanf(instruction.data(), "rotate %*s %*c=%d by %d",
				&index,
				&amount);
			assert(scanned == 2);
			(void)scanned;

			assert(amount > -1);

			for (int32_t y = 0; y < currentBuffer.GetHeight(); y++)
			{
				int32_t rotatedY = (y + amount) % currentBuffer.GetHeight();
				currentBuffer(index, rotatedY) = prevBuffer(index, y);
			}
		}
		else
		{
			assert(false);
		}
	}

	return currentBufferIndex;
}

void Puzzle08_A_2016()
{
	MemArenaConfig cfg = {};
	cfg.LargeBlockRegionSize = 1 * 1024;

	MemArena_Configure(cfg);
	{
		array<uArrayMap2D, 2> displayBuffers =
		{
			uArrayMap2D{ Vec2Int{}, 50, 6, '.'},
			uArrayMap2D{ Vec2Int{}, 50, 6, '.'}
		};

		size_t currentBufferIndex = UpdateBuffers(&displayBuffers);
		int32_t answer = displayBuffers[currentBufferIndex].Count('#');

		PuzzleOutput::Submit(2016, 8, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle08_B_2016()
{
	MemArenaConfig cfg = {};
	cfg.LargeBlockRegionSize = 2 * 1024;

	MemArena_Configure(cfg);
	{
		array<uArrayMap2D, 2> displayBuffers =
		{
			uArrayMap2D{ Vec2Int{}, 50, 6, ' '},
			uArrayMap2D{ Vec2Int{}, 50, 6, ' '}
		};

		size_t currentBufferIndex = UpdateBuffers(&displayBuffers);
		const uArrayMap2D& imageBuffer = displayBuffers[currentBufferIndex];

		string image;
		image.reserve(512);
		for (int32_t y : imageBuffer.AxisRangeY())
		{
			image.push_back('\n');
			for (int32_t x : imageBuffer.AxisRangeX())
			{
				image.push_back(imageBuffer(x, y));
			}
		}

		PuzzleOutput::Submit(2016, 8, 2, image.c_str());
	}
	MemArena_Reset();
}

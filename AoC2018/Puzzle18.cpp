#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle18_2018_Types
{
	// Lightweight wrapper to stop us having to manage current buffer index and update frame
	struct DoubleBufferArrayMap
	{
		vector<uArrayMap2D>& Buffers;
		size_t CurrentIndex = 0;
		int32_t Frame = 0;

		const uArrayMap2D& Current() const
		{
			return Buffers[CurrentIndex];
		}

		void Update();
	};
}

using namespace Puzzle18_2018_Types;

static pair<int32_t, int32_t> CountTreesAndLumber(const uArrayMap2D& lumberArea, const Vec2Int& origin)
{
	pair<int32_t, int32_t> ret = { 0, 0 };
	for (const Vec2Int& dir : Vec2Int::CardinalAndDiagonalDirections())
	{
		switch (lumberArea(origin + dir))
		{
		case '|':
			ret.first++;
			break;
		case '#':
			ret.second++;
			break;
		}
	}
	return ret;
}

void DoubleBufferArrayMap::Update()
{
	const uArrayMap2D& currentBuffer = Buffers[CurrentIndex];
	uArrayMap2D& nextBuffer = Buffers[1 - CurrentIndex];

	for (const auto& kvp : currentBuffer.Grid())
	{
		auto [trees, lumber] = CountTreesAndLumber(currentBuffer, kvp.first);
		switch (kvp.second)
		{
		case '.':
			nextBuffer(kvp.first) = trees >= 3 ? '|' : '.';
			break;
		case '|':
			nextBuffer(kvp.first) = lumber >= 3 ? '#' : '|';
			break;
		case '#':
			{
				nextBuffer(kvp.first) = trees && lumber ? '#' : '.';
			}
			break;
		}
	}

	CurrentIndex = 1 - CurrentIndex;
	Frame++;
}

static void CopyFromTo(const DoubleBufferArrayMap& from, DoubleBufferArrayMap* to)
{
	to->Buffers[to->CurrentIndex] = from.Buffers[from.CurrentIndex];
	to->Frame = from.Frame;
}

void Puzzle18_A_2018()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 8 * 1024;

	MemArena_Configure(cfg);
	{
		vector<uArrayMap2D> buffers = { ReaduArrayMap(' ') };
		buffers.push_back(buffers[0]);
		DoubleBufferArrayMap lumberArea{ buffers };

		for (size_t i = 0; i < 10; i++)
		{
			lumberArea.Update();
		}

		int32_t answer = lumberArea.Current().Count('|') * lumberArea.Current().Count('#');
		PuzzleOutput::Submit(2018, 18, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle18_B_2018()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 16 * 1024;

	MemArena_Configure(cfg);
	{
		// Four copies. Double-buffered tortoise & hare
		vector<uArrayMap2D> tortoiseBuffers = { ReaduArrayMap(' ') };
		tortoiseBuffers.push_back(tortoiseBuffers[0]);
		DoubleBufferArrayMap tortoise{ tortoiseBuffers };

		vector<uArrayMap2D> hareBuffers;
		hareBuffers.push_back(tortoiseBuffers[0]);
		hareBuffers.push_back(tortoiseBuffers[0]);
		DoubleBufferArrayMap hare{ hareBuffers };

		// First part of Brent's algorithm to detect cycle
		size_t power = 1;
		size_t lam = 1;
		hare.Update();

		while (hare.Current().GetData() != tortoise.Current().GetData())
		{
			if (power == lam)
			{
				CopyFromTo(hare, &tortoise);
				power *= 2;
				lam = 0;
			}
			hare.Update();
			lam++;
		}

		int32_t cycleLength = hare.Frame - tortoise.Frame;
		int32_t cycleOffset = (1000000000 - tortoise.Frame) % cycleLength;
		int32_t requiredFrame = tortoise.Frame + cycleOffset;
		while (tortoise.Frame != requiredFrame)
		{
			tortoise.Update();
		}

		const vector<char>& lumberArea = tortoise.Current().GetData();

		int32_t answer = static_cast<int32_t>(ranges::count(lumberArea, '|') * ranges::count(lumberArea, '#'));

		PuzzleOutput::Submit(2018, 18, 2, answer);
	}
	MemArena_Reset();
}

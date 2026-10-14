#include "stdafx.h"

using namespace std;

namespace Puzzle15_2021_Types
{
	struct Cavern
	{
		vector<int8_t> Data;
		int32_t TileWidth = 0;
		int32_t TileHeight = 0;
		int32_t CaveWidth = 0;
		int32_t CaveHeight = 0;
		array<int8_t, 20> Wrapped = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1 };

		int8_t operator()(const Vec2Short& p) const
		{
			int32_t subtileX;
			int32_t tileX = Hardware::DivModRem(p.X, TileWidth, &subtileX);

			int32_t subtileY;
			int32_t tileY = Hardware::DivModRem(p.Y, TileHeight, &subtileY);

			int8_t base = Data[(subtileY * TileWidth) + subtileX];
			int8_t adjusted = static_cast<int8_t>(base + tileX + tileY);
			assert(adjusted < Wrapped.size());

			return Wrapped[adjusted];
		}

		bool IsIn(const Vec2Short& p) const
		{
			return p.X >= 0
				&& p.X < CaveWidth
				&& p.Y >= 0
				&& p.Y < CaveHeight;
		}
	};

	struct VisitedBitfield
	{
		int32_t PaddedWidth;
		int32_t Height;
		vector<uint32_t> Data;

		VisitedBitfield(int32_t width, int32_t height)
			: PaddedWidth((width + 31) & ~31)
			, Height(height)
			, Data((PaddedWidth * Height) >> 5)
		{
		}

		bool SetVisited(const Vec2Short& p)
		{
			uint32_t index = (p.Y * PaddedWidth) + p.X;
			uint32_t wordIndex = index >> 5;
			uint32_t bitIndex = index & 0x1f;
			uint32_t bit = 1u << bitIndex;

			uint32_t current = Data[wordIndex];
			bool wasAlreadyVisited = current & bit;

			Data[wordIndex] |= bit;

			return wasAlreadyVisited;
		}
	};
}

using namespace Puzzle15_2021_Types;

static Cavern ReadRiskLevel(int32_t repeatCount)
{
	const size_t expectedCavernSize = 100;

	Cavern cavern;
	cavern.Data.reserve(expectedCavernSize * expectedCavernSize);

	int32_t tileWidth = 0;
	int32_t tileHeight = 0;
	while (PuzzleInput::NextLine())
	{
		tileWidth = 0;
		for (char c : Parse::ReadUntilSeen('\n'))
		{
			cavern.Data.push_back(c - '0');
			tileWidth++;
		}
		tileHeight++;
	}

	cavern.TileWidth = tileWidth;
	cavern.TileHeight = tileHeight;
	cavern.CaveWidth = cavern.TileWidth * repeatCount;
	cavern.CaveHeight = cavern.TileHeight * repeatCount;

	return cavern;
}

static int32_t FindLowestRisk(const Cavern& cavern)
{
	const Vec2Short start{};
	const Vec2Short finish{ static_cast<int16_t>(cavern.CaveWidth - 1), static_cast<int16_t>(cavern.CaveHeight - 1) };

	const size_t numSearchBuckets = 10;
	const size_t bucketSize = 1024;

	array<vector<Vec2Short>, numSearchBuckets> searchQueues;
	ranges::for_each(searchQueues, [&](auto& queue) { queue.reserve(bucketSize); });
	searchQueues[0].push_back(start);

	VisitedBitfield visited(cavern.CaveWidth, cavern.CaveHeight);

	for (int32_t currentRisk = 0; currentRisk < numeric_limits<int32_t>::max(); currentRisk++)
	{
		vector<Vec2Short>& currentQueue = searchQueues[currentRisk % searchQueues.size()];

		for (const Vec2Short& currentPos : currentQueue)
		{
			if (currentPos == finish)
			{
				return currentRisk;
			}

			if (visited.SetVisited(currentPos))
			{
				continue;
			}

			for (const Vec2Short& dir : Vec2Short::CardinalDirections())
			{
				const Vec2Short neighbour = currentPos + dir;
				if (cavern.IsIn(neighbour))
				{
					int32_t neighbourRisk = currentRisk + cavern(neighbour);
					searchQueues[neighbourRisk % searchQueues.size()].push_back(neighbour);
				}
			}
		}

		currentQueue.clear();
	}

	return -1;
}

void Puzzle15_A_2021()
{
	const Cavern cavern = ReadRiskLevel(1);
	int32_t answer = FindLowestRisk(cavern);

	PuzzleOutput::Submit(2021, 15, 1, answer);
}

void Puzzle15_B_2021()
{
	const Cavern cavern = ReadRiskLevel(5);
	int32_t answer = FindLowestRisk(cavern);

	PuzzleOutput::Submit(2021, 15, 2, answer);
}

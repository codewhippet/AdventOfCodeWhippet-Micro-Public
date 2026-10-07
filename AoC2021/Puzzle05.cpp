#include "stdafx.h"

using namespace std;

namespace Puzzle05_2021_Types
{
	struct AABB
	{
		Vec2Int Begin;
		Vec2Int End;

		bool Contains(const Vec2Int& v) const
		{
			if (v.X < Begin.X)
				return false;
			if (v.X >= End.X)
				return false;
			if (v.Y < Begin.Y)
				return false;
			if (v.Y >= End.Y)
				return false;

			return true;
		}
	};
}

using namespace Puzzle05_2021_Types;

static vector<pair<Vec2Int, Vec2Int>> ReadCoords()
{
	const size_t expectedNumCoords = 500;

	vector<pair<Vec2Int, Vec2Int>> coords;
	coords.reserve(expectedNumCoords);

	while (PuzzleInput::NextLine())
	{
		coords.push_back({ { Parse::GetInt32(), Parse::GetInt32() }, { Parse::GetInt32(), Parse::GetInt32() } });
	}

	return coords;
}

void Puzzle05_A_2021()
{
	vector<pair<Vec2Int, Vec2Int>> coords = ReadCoords();

	const int32_t windowSize = 250;
	vector<int16_t> windowBuffer(windowSize * windowSize);
	ArrayWrapper2D<int16_t> window(windowBuffer.data(), windowSize, windowSize);

	const int32_t expectedMaxCoord = 1000;
	static_assert((expectedMaxCoord % windowSize) == 0);
	const int32_t numWindows = expectedMaxCoord / windowSize;

	int32_t answer = 0;
	for (int32_t windowY = 0; windowY < numWindows; windowY++)
	{
		for (int32_t windowX = 0; windowX < numWindows; windowX++)
		{
			const Vec2Int windowOrigin{ windowX * windowSize, windowY * windowSize };
			const Vec2Int windowEnd = windowOrigin + Vec2Int{ windowSize, windowSize };
			const AABB windowArea{ windowOrigin, windowEnd };
			
			for (const auto& line : coords)
			{
				if ((line.first.X == line.second.X) || (line.first.Y == line.second.Y))
				{
					for (const auto& p : uLineInclusiveRange{ line.first, line.second })
					{
						if (windowArea.Contains(p))
						{
							window(p - windowOrigin)++;
						}
					}
				}
			}

			answer += static_cast<int32_t>(ranges::count_if(windowBuffer, [](int16_t count) { return count > 1; }));

			ranges::fill(windowBuffer, int16_t{ 0 });
		}
	}

	PuzzleOutput::Submit(2021, 5, 1, answer);
}

void Puzzle05_B_2021()
{
	vector<pair<Vec2Int, Vec2Int>> coords = ReadCoords();

	const int32_t windowSize = 250;
	vector<int16_t> windowBuffer(windowSize * windowSize);
	ArrayWrapper2D<int16_t> window(windowBuffer.data(), windowSize, windowSize);

	const int32_t expectedMaxCoord = 1000;
	static_assert((expectedMaxCoord % windowSize) == 0);
	const int32_t numWindows = expectedMaxCoord / windowSize;

	int32_t answer = 0;
	for (int32_t windowY = 0; windowY < numWindows; windowY++)
	{
		for (int32_t windowX = 0; windowX < numWindows; windowX++)
		{
			const Vec2Int windowOrigin{ windowX * windowSize, windowY * windowSize };
			const Vec2Int windowEnd = windowOrigin + Vec2Int{ windowSize, windowSize };
			const AABB windowArea{ windowOrigin, windowEnd };
			
			for (const auto& line : coords)
			{
				for (const auto& p : uLineInclusiveRange{ line.first, line.second })
				{
					if (windowArea.Contains(p))
					{
						window(p - windowOrigin)++;
					}
				}
			}

			answer += static_cast<int32_t>(ranges::count_if(windowBuffer, [](int16_t count) { return count > 1; }));

			ranges::fill(windowBuffer, int16_t{ 0 });
		}
	}

	PuzzleOutput::Submit(2021, 5, 2, answer);
}

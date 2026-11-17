#include "stdafx.h"

using namespace std;

namespace Puzzle08_2022_Types
{
}

using namespace Puzzle08_2022_Types;

static char DoMaxCalc(char height, char* max)
{
    char ret = *max;
    if (height > *max)
    {
        *max = height;
    }
    return ret;
}

static uArrayMap2D GenerateMaxHeights(const uArrayMap2D& heights, const Vec2Int& startAt, const Vec2Int& majorAxis, const Vec2Int& maxDirection)
{
	uArrayMap2D maxHeights{ uArrayMap2DOptions::CloneAsInvalid, heights };

	Vec2Int lineStart = startAt;
	do
	{
		char maxHeight = -1;
		Vec2Int currentPos = lineStart;
		do
		{
			maxHeights(currentPos) = DoMaxCalc(heights(currentPos), &maxHeight);
			currentPos += maxDirection;
		}
		while (heights.IsInside(currentPos));

		lineStart += majorAxis;
	}
	while (heights.IsInside(lineStart));

	return maxHeights;
}

static uArrayMap2D GenerateNearestBlockingTrees(const uArrayMap2D& heights, const Vec2Int& startAt, const Vec2Int& majorAxis, const Vec2Int& maxDirection)
{
	uArrayMap2D nearestBlocker{ uArrayMap2DOptions::CloneAsInvalid, heights };

	vector<pair<char, char>> heightStack;
	heightStack.reserve(max(heights.GetWidth(), heights.GetHeight()) + 1);
	heightStack.push_back({ numeric_limits<char>::max(), static_cast<char>(0) });

	Vec2Int lineStart = startAt;
	do
	{
		heightStack.resize(1);

		char distance = 0;
		for (Vec2Int currentPos = lineStart; heights.IsInside(currentPos); currentPos += maxDirection)
		{
			char height = heights(currentPos);
			while (heightStack.back().first < height)
				heightStack.pop_back();

			nearestBlocker(currentPos) = distance - heightStack.back().second;
			assert(nearestBlocker(currentPos) >= 0);

			heightStack.push_back({ height, distance++ });
		}

		lineStart += majorAxis;
	}
	while (heights.IsInside(lineStart));

	return nearestBlocker;
}

void Puzzle08_A_2022()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 64 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D heights = ReaduArrayMap();

		uArrayMap2D leftToRightMax = GenerateMaxHeights(heights, Vec2Int{ 0, 0 }, Vec2Int::Down(), Vec2Int::Right());
		uArrayMap2D rightToLeftMax = GenerateMaxHeights(heights, Vec2Int{ heights.GetWidth() - 1, 0 }, Vec2Int::Down(), Vec2Int::Left());
		uArrayMap2D topToBottomMax = GenerateMaxHeights(heights, Vec2Int{ 0, 0 }, Vec2Int::Right(), Vec2Int::Down());
		uArrayMap2D bottomToTopMax = GenerateMaxHeights(heights, Vec2Int{ 0, heights.GetHeight() - 1 }, Vec2Int::Right(), Vec2Int::Up());

		int32_t answer = 0;
		for (const auto &p : heights.Grid())
		{
			if ((p.second > leftToRightMax(p.first)) ||
				(p.second > rightToLeftMax(p.first)) ||
				(p.second > topToBottomMax(p.first)) ||
				(p.second > bottomToTopMax(p.first)))
			{
				answer++;
			}
		}

		PuzzleOutput::Submit(2022, 8, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle08_B_2022()
{
	MemArenaConfig cfg;
	cfg.LargeBlockRegionSize = 64 * 1024;

	MemArena_Configure(cfg);
	{
		uArrayMap2D heights = ReaduArrayMap();

		uArrayMap2D leftToRightNearest = GenerateNearestBlockingTrees(heights, Vec2Int{ 0, 0 }, Vec2Int::Down(), Vec2Int::Right());
		uArrayMap2D rightToLeftNearest = GenerateNearestBlockingTrees(heights, Vec2Int{ heights.GetWidth() - 1, 0 }, Vec2Int::Down(), Vec2Int::Left());
		uArrayMap2D topToBottomNearest = GenerateNearestBlockingTrees(heights, Vec2Int{ 0, 0 }, Vec2Int::Right(), Vec2Int::Down());
		uArrayMap2D bottomToTopNearest = GenerateNearestBlockingTrees(heights, Vec2Int{ 0, heights.GetHeight() - 1 }, Vec2Int::Right(), Vec2Int::Up());

		MaxValue<int32_t> answer;
		for (const auto &p : heights.Grid())
		{
			int32_t bottomToTop = bottomToTopNearest(p.first);
			int32_t topToBottom = topToBottomNearest(p.first);
			int32_t leftToRight = leftToRightNearest(p.first);
			int32_t rightToLeft = rightToLeftNearest(p.first);

			int32_t scenicScore =
				leftToRight *
				rightToLeft *
				topToBottom *
				bottomToTop;

			answer.Update(scenicScore);
		}

		PuzzleOutput::Submit(2022, 8, 2, answer);
	}
	MemArena_Reset();
}

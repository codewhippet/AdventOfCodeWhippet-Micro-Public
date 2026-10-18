#include "stdafx.h"

using namespace std;

namespace Puzzle17_2021_Types
{
}

using namespace Puzzle17_2021_Types;

static pair<Vec2Int, Vec2Int> ReadTargetArea()
{
	int32_t x1 = Parse::GetInt32();
	int32_t x2 = Parse::GetInt32();
	int32_t y1 = Parse::GetInt32();
	int32_t y2 = Parse::GetInt32();

	return
	{
		{ max(x1, x2), max(y1, y2) },
		{ min(x1, x2), min(y1, y2) },
	};
}

static int32_t TriangleNumber(int32_t n)
{
	return (n * (n + 1)) / 2;
}

static bool HasHitTarget(const Vec2Int& probe, const pair<Vec2Int, Vec2Int>& target)
{
	return
		(probe.X <= target.first.X) &&
		(probe.X >= target.second.X) &&
		(probe.Y <= target.first.Y) &&
		(probe.Y >= target.second.Y);
}

static bool DoesHitTarget(Vec2Int velocity, const pair<Vec2Int, Vec2Int>& target)
{
	Vec2Int probe{ 0, 0 };

	while (true)
	{
		if (HasHitTarget(probe, target))
			return true;

		if (probe.Y < target.second.Y)
			return false;

		if (probe.X > target.first.X)
			return false;

		// Move
		probe = probe + velocity;

		// Drag
		if (velocity.X != 0)
		{
			velocity.X += velocity.X > 0 ? -1 : 1;
		}

		// Gravity
		velocity.Y--;
	}

	return false;
}

void Puzzle17_A_2021()
{
	pair<Vec2Int, Vec2Int> target = ReadTargetArea();

	int32_t answer = TriangleNumber(abs(target.second.Y) - 1);

	PuzzleOutput::Submit(2021, 17, 1, answer);
}

void Puzzle17_B_2021()
{
	pair<Vec2Int, Vec2Int> target = ReadTargetArea();

	// Assume the target is down and to the right
	int32_t maximumY = abs(target.second.Y) - 1;
	int32_t minimumY = target.second.Y;

	int32_t maximumX = target.first.X;
	// Minimum X is the nth triangular number that doesn't exceed the left edge
	int32_t minimumX = static_cast<int32_t>((sqrtf(8.0f * target.second.X + 1.0f) - 1) * 0.5f);

	int32_t hits = 0;
	for (int32_t y = minimumY; y <= maximumY; y++)
	{
		for (int32_t x = minimumX; x <= maximumX; x++)
		{
			if (DoesHitTarget({ x, y }, target))
				hits++;
		}
	}

	int32_t answer = hits;
	PuzzleOutput::Submit(2021, 17, 2, answer);
}

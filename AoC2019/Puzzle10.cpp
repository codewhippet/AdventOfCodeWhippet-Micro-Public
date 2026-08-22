#include "stdafx.h"
#include <numbers>

using namespace std;

namespace Puzzle10_2019_Types
{
	template <typename TYPE>
	class AreaAroundZero
	{
	public:
		AreaAroundZero(int32_t size)
			: Size(size)
			, Stride((size * 2) + 1)
			, Data(Stride* Stride)
		{
		}

	protected:
		int32_t Index(const Vec2Int& v) const
		{
			int32_t x = v.X + Size;
			int32_t y = v.Y + Size;
			int32_t index = (y * Stride) + x;
			return index;
		}

		int32_t Size;
		int32_t Stride;
		vector<TYPE> Data;
	};

	class GcdCache : private AreaAroundZero<int32_t>
	{
	public:
		GcdCache(int32_t size)
			: AreaAroundZero(size)
		{
		}

		int32_t Get(const Vec2Int& v)
		{
			int32_t index = Index(v);

			int32_t scale = Data[index];
			if (scale == 0)
			{
				scale = gcd(v.X, v.Y);
				Data[index] = scale;
			}

			return scale;
		}
	};

	class SeenBuffer : private AreaAroundZero<char>
	{
	public:
		SeenBuffer(int32_t size)
			: AreaAroundZero(size)
		{
		}

		bool HasSeen(const Vec2Int& v)
		{
			int32_t index = Index(v);

			bool seenBefore = Data[index];
			if (!seenBefore)
			{
				Data[index] = 1;
			}

			return seenBefore;
		}

		void Reset()
		{
			memset(Data.data(), 0, Data.size());
		}
	};

	struct DestroyedTracker
	{
		int32_t TargetsLeft;
		Vec2Int LastDestroyed;

		void Update(const Vec2Int& pos)
		{
			if (--TargetsLeft == 0)
			{
				LastDestroyed = pos;
			}
		}
	};
}

using namespace Puzzle10_2019_Types;

static float AngleFromDirection(const Vec2Int& dir)
{
	float angle = atan2f(static_cast<float>(dir.X), static_cast<float>(-dir.Y));
	return (angle >= 0.0) ? angle : (2.0f * numbers::pi_v<float> + angle);
}

void Puzzle10_A_2019()
{
	vector<Vec2Int> asteroids;
	asteroids.reserve(512);

	Vec2Int pos{};
	MaxValue<int32_t> maxX;
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case '#':
			asteroids.push_back(pos);
			pos.X++;
			break;

		case '\n':
			pos.X = 0;
			pos.Y++;
			break;

		default:
			pos.X++;
			break;
		}

		maxX.Update(pos.X);
	}

	const int32_t width = maxX.Get();
	assert(width == pos.Y);

	MaxValue<int32_t> mostVisible;

	GcdCache gcdCache(width);
	SeenBuffer seen(width);
	for (size_t i = 0; i < asteroids.size(); i++)
	{
		seen.Reset();

		int32_t visible = 0;
		for (size_t j = 0; j < asteroids.size(); j++)
		{
			if (i == j)
				continue;

			Vec2Int heading = asteroids[j] - asteroids[i];
			int32_t scale = gcdCache.Get(heading);
			if (scale != 1)
			{
				heading.X /= scale;
				heading.Y /= scale;
			}

			if (seen.HasSeen(heading) == false)
			{
				visible++;
			}
		}

		mostVisible.Update(visible);
	}

	int32_t answer = mostVisible.Get();
	PuzzleOutput::Submit(2019, 10, 1, answer);
}

void Puzzle10_B_2019()
{
	vector<Vec2Int> asteroids;
	asteroids.reserve(512);

	Vec2Int pos{};
	MaxValue<int32_t> maxX;
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case '#':
			asteroids.push_back(pos);
			pos.X++;
			break;

		case '\n':
			pos.X = 0;
			pos.Y++;
			break;

		default:
			pos.X++;
			break;
		}

		maxX.Update(pos.X);
	}

	const int32_t width = maxX.Get();
	assert(width == pos.Y);

	int32_t maxVisible = 0;
	size_t bestIndex = 0;

	GcdCache gcdCache(width);

	{
		SeenBuffer seen(width);
		for (size_t i = 0; i < asteroids.size(); i++)
		{
			seen.Reset();

			int32_t visible = 0;
			for (size_t j = 0; j < asteroids.size(); j++)
			{
				if (i == j)
					continue;

				Vec2Int heading = asteroids[j] - asteroids[i];
				int32_t scale = gcdCache.Get(heading);
				if (scale != 1)
				{
					heading.X /= scale;
					heading.Y /= scale;
				}

				if (seen.HasSeen(heading) == false)
				{
					visible++;
				}
			}

			if (visible > maxVisible)
			{
				maxVisible = visible;
				bestIndex = i;
			}
		}
	}

	const Vec2Int stationLocation = asteroids[bestIndex];

	vector<tuple<float, int32_t, int32_t>> targetOrdering;
	targetOrdering.reserve(asteroids.size() - 1);
	for (int32_t i = 0; i < static_cast<int32_t>(asteroids.size()); i++)
	{
		if (i == bestIndex)
			continue;

		Vec2Int heading = asteroids[i] - stationLocation;
		int32_t scale = gcdCache.Get(heading);
		if (scale != 1)
		{
			heading.X /= scale;
			heading.Y /= scale;
		}

		float headingAngle = AngleFromDirection(heading);
		int32_t distanceToTarget = ManhattanDistance(asteroids[i], stationLocation);

		targetOrdering.push_back({ headingAngle, distanceToTarget, i });
	}

	ranges::sort(targetOrdering);

	DestroyedTracker tracker;
	tracker.TargetsLeft = 200;

	float lastHeadingDestroyed = -1.0f;
	for (size_t i = 0; tracker.TargetsLeft > 0; i = ((i + 1) == targetOrdering.size() ? 0 : i + 1))
	{
		if ((get<0>(targetOrdering[i]) == lastHeadingDestroyed) || (get<2>(targetOrdering[i]) == -1))
			continue;

		tracker.Update(asteroids[get<2>(targetOrdering[i])]);
		lastHeadingDestroyed = get<0>(targetOrdering[i]);
		get<2>(targetOrdering[i]) = -1;
	}

	int32_t answer = tracker.LastDestroyed.X * 100 + tracker.LastDestroyed.Y;
	PuzzleOutput::Submit(2019, 10, 2, answer);
}

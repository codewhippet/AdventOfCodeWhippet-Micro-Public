#include "stdafx.h"
#include <math.h>

using namespace std;

namespace Puzzle10_2018_Types
{
	struct Particle
	{
		Vec2Int Position;
		Vec2Int Velocity;
	};
}

using namespace Puzzle10_2018_Types;

static vector<Particle> ReadParticles()
{
	vector<Particle> particles;
	particles.reserve(512);

	while (PuzzleInput::NextLine())
	{
		Particle p;

		p.Position.X = Parse::GetInt32();
		p.Position.Y = Parse::GetInt32();
		p.Velocity.X = Parse::GetInt32();
		p.Velocity.Y = Parse::GetInt32();
		PuzzleInput::DropLine();

		particles.push_back(p);
	}
	return particles;
}

static int32_t HeightAt(const vector<Particle>& particles, int32_t time)
{
	MinMaxValues<int32_t> height;
	for (const Particle& p : particles)
	{
		height.Update(p.Position.Y + p.Velocity.Y * time);
	}
	return height.GetMax() - height.GetMin();
}

static int32_t FindMinimumHeightTime(const vector<Particle>& particles)
{
	float totalTime = 0;
	int32_t totalCount = 0;
	for (const Particle& p : particles)
	{
		if (p.Velocity.X != 0)
		{
			totalTime += abs((float)p.Position.X / p.Velocity.X);
			totalCount++;
		}
		if (p.Velocity.Y != 0)
		{
			totalTime += abs((float)p.Position.Y / p.Velocity.Y);
			totalCount++;
		}
	}

	int32_t approximateTime = lround(totalTime / totalCount);
	int32_t searchBegin = approximateTime - 10;
	int32_t searchEnd = approximateTime + 10;

	int32_t minHeight = numeric_limits<int32_t>::max();
	int32_t minHeightTime = -1;
	for (int32_t i = searchBegin; i < searchEnd; i++)
	{
		int32_t height = HeightAt(particles, i);
		if (height < minHeight)
		{
			minHeight = height;
			minHeightTime = i;
		}
	}

	return minHeightTime;
}

void Puzzle10_A_2018()
{
	vector<Particle> particles = ReadParticles();
	int32_t minHeightTime = FindMinimumHeightTime(particles);

	HashSet<Vec2Int> points(1024, Vec2Int{ -1, -1 });
	for (const Particle& p : particles)
	{
		points.Insert(p.Position + p.Velocity * minHeightTime);
	}

	Vec2Int min = accumulate(points.begin(), points.end(), Vec2Int::Max(), [](const Vec2Int& a, const Vec2Int& b) { return Vec2Int::MinElements(a, b); });
	Vec2Int max = accumulate(points.begin(), points.end(), Vec2Int::Min(), [](const Vec2Int& a, const Vec2Int& b) { return Vec2Int::MaxElements(a, b); });

	Vec2Int size = max - min + Vec2Int{ 1, 1 };

	string answer;
	answer.reserve((size.X + 1) * size.Y + 1);
	
	for (int32_t y = min.Y; y <= max.Y; y++)
	{
		answer += '\n';
		for (int32_t x = min.X; x <= max.X; x++)
		{
			answer += points.Contains({ x, y }) ? '#' : '.';
		}
	}

	PuzzleOutput::Submit(2018, 10, 1, answer.c_str());
}

void Puzzle10_B_2018()
{
	vector<Particle> particles = ReadParticles();
	int32_t answer = FindMinimumHeightTime(particles);

	PuzzleOutput::Submit(2018, 10, 2, answer);
}

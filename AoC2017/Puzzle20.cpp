#include "stdafx.h"

using namespace std;

#define static

namespace Puzzle20_2017_Types
{
	struct Particle
	{
		Vec3Int Position;
		Vec3Int Velocity;
		Vec3Int Acceleration;
		bool Dead = false;
	};
}

using namespace Puzzle20_2017_Types;

void Puzzle20_A_2017()
{
	int32_t answer = 0;

	int32_t particleIndex = 0;
	int32_t smallestAcceleration = numeric_limits<int32_t>::max();
	while (PuzzleInput::NextLine())
	{
		// Ignore position and velocity
		Parse::GetInt32(); Parse::GetInt32(); Parse::GetInt32();
		Parse::GetInt32(); Parse::GetInt32(); Parse::GetInt32();

		int32_t acceleration = abs(Parse::GetInt32()) + abs(Parse::GetInt32()) + abs(Parse::GetInt32());
		if (acceleration < smallestAcceleration)
		{
			smallestAcceleration = acceleration;
			answer = particleIndex;
		}
		particleIndex++;

		PuzzleInput::DropChar(); // Trailing '>'
	}

	PuzzleOutput::Submit(2017, 20, 1, answer);
}

void Puzzle20_B_2017()
{
	vector<Particle> particles;
	particles.reserve(1000);
	while (PuzzleInput::NextLine())
	{
		particles.push_back(Particle{
			Vec3Int{ Parse::GetInt32(), Parse::GetInt32(), Parse::GetInt32() },
			Vec3Int{ Parse::GetInt32(), Parse::GetInt32(), Parse::GetInt32() },
			Vec3Int{ Parse::GetInt32(), Parse::GetInt32(), Parse::GetInt32() },
			});

		PuzzleInput::DropChar(); // Trailing '>'
	}

	size_t particlesAlive = particles.size();
	for (size_t frame = 0; frame < 50; frame++)
	{
		// Collide
		HashMap<Vec3Int, size_t> collisionLocations(2 * 1024, Vec3Int{ numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max(), numeric_limits<int32_t>::max() });
		for (size_t particleIndex = 0; particleIndex < particlesAlive; particleIndex++)
		{
			if (collisionLocations.Insert(particles[particleIndex].Position, particleIndex) == false)
			{
				particles[particleIndex].Dead = true;
				particles[collisionLocations.At(particles[particleIndex].Position)].Dead = true;
			}
		}

		// Simulate
		Particle* dst = &particles[0];
		Particle* end = dst + particlesAlive;
		for (Particle* src = dst; src != end; src++)
		{
			if (src->Dead)
				continue;

			dst->Acceleration = src->Acceleration;
			dst->Velocity = src->Velocity + dst->Acceleration;
			dst->Position = src->Position + dst->Velocity;
			dst->Dead = false;
			dst++;
		}

		particlesAlive = distance(&particles[0], dst);
	}

	int32_t answer = static_cast<int32_t>(particlesAlive);

	PuzzleOutput::Submit(2017, 20, 2, answer);
}

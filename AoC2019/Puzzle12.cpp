#include "stdafx.h"
#include <numeric>

using namespace std;

namespace Puzzle12_2019_Types
{
	struct Moon
	{
		Vec3Int Position;
		Vec3Int Velocity;
	};
}

using namespace Puzzle12_2019_Types;

static int32_t PotentialEnergy(const Moon& m)
{
	return abs(m.Position.X) + abs(m.Position.Y) + abs(m.Position.Z);
}

static int32_t KineticEnergy(const Moon& m)
{
	return abs(m.Velocity.X) + abs(m.Velocity.Y) + abs(m.Velocity.Z);
}

static int32_t TotalEnergyNM(const Moon& m)
{
	return PotentialEnergy(m) * KineticEnergy(m);
}

static int32_t GetX(const Vec3Int& v) { return v.X; }
static int32_t GetY(const Vec3Int& v) { return v.Y; }
static int32_t GetZ(const Vec3Int& v) { return v.Z; }

static void SetX(Vec3Int* v, int32_t x) { v->X = x; }
static void SetY(Vec3Int* v, int32_t y) { v->Y = y; }
static void SetZ(Vec3Int* v, int32_t z) { v->Z = z; }

template<typename GETTER, typename SETTER>
void ApplyGravity(const pair<Moon*, Moon*>& p, const GETTER& get, const SETTER& set)
{
	Moon* a = p.first;
	Moon* b = p.second;

	if (get(a->Position) < get(b->Position))
	{
		set(&a->Velocity, get(a->Velocity) + 1);
		set(&b->Velocity, get(b->Velocity) - 1);
	}
	else if (get(a->Position) > get(b->Position))
	{
		set(&a->Velocity, get(a->Velocity) - 1);
		set(&b->Velocity, get(b->Velocity) + 1);
	}
}

static void ApplyGravityXYZ(const pair<Moon*, Moon*>& p)
{
	ApplyGravity(p, GetX, SetX);
	ApplyGravity(p, GetY, SetY);
	ApplyGravity(p, GetZ, SetZ);
}

template<typename GETTER, typename SETTER>
void ApplyVelocity(Moon* m, const GETTER& get, const SETTER& set)
{
	set(&m->Position, get(m->Position) + get(m->Velocity));
}

static void ApplyVelocityXYZ(Moon* m)
{
	ApplyVelocity(m, GetX, SetX);
	ApplyVelocity(m, GetY, SetY);
	ApplyVelocity(m, GetZ, SetZ);
}

template<typename GETTER, typename SETTER>
static int32_t FindCycleLength(vector<Moon> moons, const GETTER& get, const SETTER& set)
{
	for (int32_t frame = 0; frame < numeric_limits<int32_t>::max() - 1; frame++)
	{
		auto applyGravityAxis = [&](const pair<Moon*, Moon*>& p)
			{
				ApplyGravity(p, get, set);
			};

		ranges::for_each(AllUnorderedPairs(moons.size())
			| views::transform([&](const auto& p) -> pair<Moon*, Moon*>
				{
					return { &moons[p.first], &moons[p.second] };
				}),
			applyGravityAxis);

		auto applyVelocityAxis = [&](Moon* m)
			{
				ApplyVelocity(m, get, set);
			};

		ranges::for_each(moons | views::transform([](Moon& m) { return &m; }), applyVelocityAxis);

		if (ranges::all_of(moons, [&](Moon& m) { return get(m.Velocity) == 0; }))
		{
			return (frame + 1) * 2;
		}
	}

	return -1;
}

void Puzzle12_A_2019()
{
	vector<Moon> moons;
	moons.reserve(4);

	while (PuzzleInput::NextLine())
	{
		Moon m;
		m.Position = Vec3Int{ Parse::GetInt32(), Parse::GetInt32(), Parse::GetInt32() };
		moons.push_back(m);

		PuzzleInput::DropLine();
	}

	for (int32_t i = 0; i < 1000; i++)
	{
		ranges::for_each(AllUnorderedPairs(moons.size())
			| views::transform([&](const auto& p) -> pair<Moon*, Moon*>
				{
					return { &moons[p.first], &moons[p.second] };
				}),
			ApplyGravityXYZ);

		ranges::for_each(moons | views::transform([](Moon& m) { return &m; }), ApplyVelocityXYZ);
	}

	auto moonEnergy = moons | views::transform(TotalEnergyNM);
	int32_t answer = accumulate(moonEnergy.begin(), moonEnergy.end(), 0);

	PuzzleOutput::Submit(2019, 12, 1, answer);
}

void Puzzle12_B_2019()
{
	vector<Moon> moons;
	moons.reserve(4);

	while (PuzzleInput::NextLine())
	{
		Moon m;
		m.Position = Vec3Int{ Parse::GetInt32(), Parse::GetInt32(), Parse::GetInt32() };
		moons.push_back(m);

		PuzzleInput::DropLine();
	}

	int64_t cycleLengthX = FindCycleLength(moons, GetX, SetX);
	int64_t cycleLengthY = FindCycleLength(moons, GetY, SetY);
	int64_t cycleLengthZ = FindCycleLength(moons, GetZ, SetZ);

	int64_t answer = lcm(lcm(cycleLengthX, cycleLengthY), cycleLengthZ);

	PuzzleOutput::Submit(2019, 12, 2, answer);
}

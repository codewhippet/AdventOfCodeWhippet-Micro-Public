#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle22_2018_Types
{
	enum : uint8_t
	{
		Neither = 0,
		Torch = 1,
		ClimbingGear = 2,

		NeitherBit = 1 << Neither,
		TorchBit = 1 << Torch,
		ClimbingGearBit = 1 << ClimbingGear,
	};

	enum : size_t
	{
		CaveWidth = 80,
		CaveHeight = 1000,

		StripHeight = 256,
	};

	struct Cave
	{
		void GenerateRows(int32_t rowEnd);

		pair<bool, int32_t> CheckMove(const Vec3Int& from, const Vec3Int& to);
		uint8_t& Cell(const Vec2Int& p);

		int32_t Depth;
		Vec2Int Target;

		array<uint8_t, CaveWidth * StripHeight> Grid = {};
		int32_t GridVerticalOffset = 0;

		array<array<uint32_t, CaveWidth>, 2> ErosionLevels = {};
		int32_t NumGeneratedRows = 0;
	};
}

using namespace Puzzle22_2018_Types;

static int32_t CalculateRiskLevel(int32_t depth, const Vec2Int& target)
{
	array<int32_t, 3> terrainCounts = {};

	array<vector<int32_t>, 2> buffers;
	buffers[0].resize(target.X + 1);
	buffers[1].resize(target.X + 1);

	for (const Vec2Int& region : uGridRange{ {}, target + Vec2Int{ 1, 1 } })
	{
		size_t bufferIndex = region.Y & 1;
		vector<int32_t>& currentRow = buffers[bufferIndex];
		const vector<int32_t>& prevRow = buffers[1 - bufferIndex];

		int32_t geologicIndex = -1;
		if (region == Vec2Int{ 0, 0 })
		{
			geologicIndex = 0;
		}
		else if (region == target)
		{
			geologicIndex = 0;
		}
		else if (region.Y == 0)
		{
			geologicIndex = region.X * 16807;
		}
		else if (region.X == 0)
		{
			geologicIndex = region.Y * 48271;
		}
		else
		{
			geologicIndex = currentRow[region.X - 1] * prevRow[region.X];
		}

		int32_t erosionLevel = (geologicIndex + depth) % 20183;
		currentRow[region.X] = erosionLevel;

		terrainCounts[erosionLevel % terrainCounts.size()]++;
	}

	return terrainCounts[1] + terrainCounts[2] * 2;
}

void Cave::GenerateRows(int32_t rowEnd)
{
	const array<uint8_t, 3> alreadyVisited =
	{
		NeitherBit, // Rocky
		TorchBit, // Wet
		ClimbingGearBit, // Narrow
	};

	for (int32_t y = NumGeneratedRows; y < rowEnd; y++)
	{
		int32_t stripY = y & (StripHeight - 1); // Wrap Y

		for (int32_t x = 0; x < CaveWidth; x++)
		{
			bool zeroX = (x == 0);
			bool zeroY = (y == 0);

			int32_t geologicIndex = -1;
			if (zeroX && zeroY)
			{
				geologicIndex = 0;
			}
			else if ((x == Target.X) && (y == Target.Y))
			{
				geologicIndex = 0;
			}
			else if (zeroY)
			{
				geologicIndex = x * 16807;
			}
			else if (zeroX)
			{
				geologicIndex = y * 48271;
			}
			else
			{
				geologicIndex = ErosionLevels[y & 1][x - 1] * ErosionLevels[(y - 1) & 1][x];
			}

			int32_t erosionLevel = (geologicIndex + Depth) % 20183;
			ErosionLevels[y & 1][x] = erosionLevel;

			Grid[(stripY * CaveWidth) + x] = alreadyVisited[erosionLevel % alreadyVisited.size()];
		}

		if (y >= NumGeneratedRows)
		{
			NumGeneratedRows = y + 1;
			GridVerticalOffset = NumGeneratedRows - StripHeight;
		}
	}
}

pair<bool, int32_t> Puzzle22_2018_Types::Cave::CheckMove(const Vec3Int& from, const Vec3Int& to)
{
	assert(abs(from.X - to.X) <= 1);
	assert(abs(from.Y - to.Y) <= 1);

	if ((to.X >= CaveWidth) || (to.Y >= CaveHeight))
	{
		// Can't move off the map
		return { false, -1 };
	}

	if (to == from)
	{
		// No point doing nothing
		return { false, -1 };
	}

	uint8_t currentRegion = Cell({ from.X, from.Y });
	uint8_t nextRegion = Cell({ to.X, to.Y });

	// Check that any equipment is both valid and not-visited for both regions
	bool bothCompatible = ((currentRegion & from.Z) == 0) && ((nextRegion & to.Z) == 0);
	if (!bothCompatible)
	{
		return { false, -1 };
	}

	// Move is within bounds and has compatible equipment
	return { true, from.Z == to.Z ? 1 : 7 };
}

uint8_t& Puzzle22_2018_Types::Cave::Cell(const Vec2Int& p)
{
	assert(p.Y >= GridVerticalOffset);
	assert(p.Y < NumGeneratedRows);
	int32_t stripY = p.Y & (StripHeight - 1);
	return Grid[(stripY * CaveWidth) + p.X];
}

static uint32_t PackState(const Vec3Int& state)
{
	return(state.X & 0xfff) << 16
		| (state.Y & 0xfff) << 4
		| (state.Z & 0xf);

}

static Vec3Int UnpackState(uint32_t state)
{
	return { (state >> 16) & 0xfff, (state >> 4) & 0xfff, state & 0xf };
}

static int32_t FindQuickestPath(Cave* cave, const Vec2Int& startingLocation)
{
	array<vector<uint32_t>, 8> searchQueues = {};
	ranges::for_each(searchQueues, [](auto& v) { v.reserve(512); });
	searchQueues[0].push_back(PackState({ startingLocation.X, startingLocation.Y, TorchBit }));

	const Vec3Int targetState{ cave->Target.X, cave->Target.Y, TorchBit };

	for (int32_t currentTime = 0; currentTime < numeric_limits<int32_t>::max(); currentTime++)
	{
		vector<uint32_t>& activeQueue = searchQueues[currentTime % searchQueues.size()];
		for (size_t i = 0; i < activeQueue.size(); i++)
		{
			Vec3Int currentState = UnpackState(activeQueue[i]);
			if (currentState == targetState)
			{
				return currentTime;
			}

			if (cave->NumGeneratedRows < currentState.Y + 2)
				cave->GenerateRows((int32_t)currentState.Y + 2);

			uint8_t& currentLocation = cave->Cell({ currentState.X, currentState.Y });
			bool alreadyVisited = (currentLocation & currentState.Z) != 0;
			if (alreadyVisited)
			{
				continue;
			}

			vector<uint32_t>& moveQueue = searchQueues[(currentTime + 1) % searchQueues.size()];
			for (const Vec2Int dir : Vec2Int::CardinalDirections())
			{
				Vec3Int nextState{ currentState.X + dir.X, currentState.Y + dir.Y, currentState.Z };
				auto [possible, nextCost] = cave->CheckMove(currentState, nextState);
				if (possible)
				{
					assert(nextCost == 1);
					moveQueue.push_back(PackState(nextState));
				}
			}

			vector<uint32_t>& changeEquipmentQueue = searchQueues[(currentTime + 7) % searchQueues.size()];
			for (int32_t equipment : array<uint8_t, 3>{ NeitherBit, TorchBit, ClimbingGearBit })
			{
				Vec3Int nextState{ currentState.X, currentState.Y, equipment };
				auto [possible, nextCost] = cave->CheckMove(currentState, nextState);
				if (possible)
				{
					assert(nextCost == 7);
					changeEquipmentQueue.push_back(PackState(nextState));
				}
			}

			currentLocation |= currentState.Z;
		}
		activeQueue.clear();
	}

	return -1;
}

void Puzzle22_A_2018()
{
	int32_t depth = Parse::GetInt32();
	PuzzleInput::NextLine();
	Vec2Int target{ Parse::GetInt32(), Parse::GetInt32() };

	int32_t answer = CalculateRiskLevel(depth, target);
	PuzzleOutput::Submit(2018, 22, 1, answer);
}

void Puzzle22_B_2018()
{
	int32_t depth = Parse::GetInt32();
	PuzzleInput::NextLine();
	Vec2Int target{ Parse::GetInt32(), Parse::GetInt32() };

	unique_ptr<Cave> strippedCave = make_unique<Cave>();
	strippedCave->Depth = depth;
	strippedCave->Target = target;
	strippedCave->GenerateRows(StripHeight);
	int32_t answer = FindQuickestPath(strippedCave.get(), {});

	PuzzleOutput::Submit(2018, 22, 2, answer);
}

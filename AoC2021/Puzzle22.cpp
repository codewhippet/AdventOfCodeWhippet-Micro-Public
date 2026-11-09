#include "stdafx.h"

using namespace std;

namespace Puzzle22_2021_Types
{
	struct Command
	{
		bool On = false;

		// Half-open range
		Vec3Int Begin;
		Vec3Int End;
	};

	struct Cube
	{
		// Half-open range
		Vec3Int Begin;
		Vec3Int End;
	};
}

using namespace Puzzle22_2021_Types;

static bool Intersect(const Cube& a, const Cube& b, Cube* clipped)
{
	clipped->Begin = Vec3Int::MaxElements(a.Begin, b.Begin);
	clipped->End = Vec3Int::MinElements(a.End, b.End);

	return
		clipped->Begin.X < clipped->End.X &&
		clipped->Begin.Y < clipped->End.Y &&
		clipped->Begin.Z < clipped->End.Z;
}

static bool Overlaps(const Cube& a, const Cube& b)
{
	if ((a.End.X <= b.Begin.X) || (a.Begin.X >= b.End.X))
		return false;
	if ((a.End.Y <= b.Begin.Y) || (a.Begin.Y >= b.End.Y))
		return false;
	if ((a.End.Z <= b.Begin.Z) || (a.Begin.Z >= b.End.Z))
		return false;

	return true;
}

static void ShardExisting(const Cube& splitter, const vector<Cube>& src, vector<Cube>* dst)
{
	for (const Cube& a : src)
	{
		if (Overlaps(a, splitter) == false)
		{
			dst->push_back(a);
			continue;
		}

		Cube clipped = a;

		// Carve slabs on X
		if (clipped.End.X <= splitter.Begin.X)
		{
			dst->push_back(clipped);
			continue;
		}
		if (clipped.Begin.X < splitter.Begin.X)
		{
			Cube slab = clipped;
			slab.End.X = splitter.Begin.X;
			dst->push_back(slab);
			clipped.Begin.X = splitter.Begin.X;
		}
		if (clipped.Begin.X >= splitter.End.X)
		{
			dst->push_back(clipped);
			continue;
		}
		if (clipped.End.X > splitter.End.X)
		{
			Cube slab = clipped;
			slab.Begin.X = splitter.End.X;
			dst->push_back(slab);
			clipped.End.X = splitter.End.X;
		}

		// Carve slabs on Y
		if (clipped.End.Y <= splitter.Begin.Y)
		{
			dst->push_back(clipped);
			continue;
		}
		if (clipped.Begin.Y < splitter.Begin.Y)
		{
			Cube slab = clipped;
			slab.End.Y = splitter.Begin.Y;
			dst->push_back(slab);
			
			clipped.Begin.Y = splitter.Begin.Y;
		}
		if (clipped.Begin.Y >= splitter.End.Y)
		{
			dst->push_back(clipped);
			continue;
		}
		if (clipped.End.Y > splitter.End.Y)
		{
			Cube slab = clipped;
			slab.Begin.Y = splitter.End.Y;
			dst->push_back(slab);
			clipped.End.Y = splitter.End.Y;
		}

		// Carve slabs on Z
		if (clipped.End.Z <= splitter.Begin.Z)
		{
			dst->push_back(clipped);
			continue;
		}
		if (clipped.Begin.Z < splitter.Begin.Z)
		{
			Cube slab = clipped;
			slab.End.Z = splitter.Begin.Z;
			dst->push_back(slab);
			
			clipped.Begin.Z = splitter.Begin.Z;
		}
		if (clipped.Begin.Z >= splitter.End.Z)
		{
			dst->push_back(clipped);
			continue;
		}
		if (clipped.End.Z > splitter.End.Z)
		{
			Cube slab = clipped;
			slab.Begin.Z = splitter.End.Z;
			dst->push_back(slab);
			clipped.End.Z = splitter.End.Z;
		}
	}
}

void Puzzle22_A_2021()
{
	const size_t numCubesToReserve = 256;

	array<vector<Cube>, 2> cubes;
	cubes[0].reserve(numCubesToReserve);
	cubes[1].reserve(numCubesToReserve);

	const Cube region{ Vec3Int{ -50, -50, -50 }, Vec3Int{ 51, 51, 51 } };

	size_t activeBuffer = 0;
	while (PuzzleInput::NextLine())
	{


		PuzzleInput::DropChar();
		bool on = PuzzleInput::GetChar() == 'n';
		(void)on;

		Cube c;
		c.Begin.X = Parse::GetInt32();
		c.End.X = Parse::GetInt32() + 1;
		c.Begin.Y = Parse::GetInt32();
		c.End.Y = Parse::GetInt32() + 1;
		c.Begin.Z = Parse::GetInt32();
		c.End.Z = Parse::GetInt32() + 1;

		Cube clipped;
		if (Intersect(c, region, &clipped))
		{
			const vector<Cube>& src = cubes[activeBuffer];
			vector<Cube>& dst = cubes[1 - activeBuffer];
			dst.clear();

			ShardExisting(clipped, src, &dst);
			if (on)
			{
				dst.push_back(clipped);
			}

			activeBuffer = 1 - activeBuffer;
		}		
	}

	int32_t answer = 0;
	for (const Cube& c : cubes[activeBuffer])
	{
		int32_t volume = 1;
		volume *= c.End.X - c.Begin.X;
		volume *= c.End.Y - c.Begin.Y;
		volume *= c.End.Z - c.Begin.Z;
		assert(volume > 0);
		answer += volume;
	}
	
	PuzzleOutput::Submit(2021, 22, 1, answer);
}

void Puzzle22_B_2021()
{
	const size_t expectedInputCommands = 420;
	const size_t numCubesToReserve = 1024;

	vector<Command> commands;
	commands.reserve(expectedInputCommands);
	while (PuzzleInput::NextLine())
	{
		PuzzleInput::DropChar();

		Command c;
		c.On = PuzzleInput::GetChar() == 'n';

		c.Begin.X = Parse::GetInt32();
		c.End.X = Parse::GetInt32() + 1;
		c.Begin.Y = Parse::GetInt32();
		c.End.Y = Parse::GetInt32() + 1;
		c.Begin.Z = Parse::GetInt32();
		c.End.Z = Parse::GetInt32() + 1;

		commands.push_back(c);
	}

	array<vector<Cube>, 2> cubes;
	cubes[0].reserve(numCubesToReserve);
	cubes[1].reserve(numCubesToReserve);

	int64_t answer = 0;

	const int32_t inf = 1000000;
	const array<Cube, 8> octants =
	{
		Cube{ {    0,    0,    0 }, { inf, inf, inf } },
		Cube{ {    0,    0, -inf }, { inf, inf,   0 } },
		Cube{ {    0, -inf,    0 }, { inf,   0, inf } },
		Cube{ {    0, -inf, -inf }, { inf,   0,   0 } },
		Cube{ { -inf,    0,    0 }, {   0, inf, inf } },
		Cube{ { -inf,    0, -inf }, {   0, inf,   0 } },
		Cube{ { -inf, -inf,    0 }, {   0,   0, inf } },
		Cube{ { -inf, -inf, -inf }, {   0,   0,   0 } },
	};

	for (const Cube& region : octants)
	{
		cubes[0].clear();
		cubes[1].clear();

		size_t activeBuffer = 0;
		for (const Command& command : commands)
		{
			Cube c{ command.Begin, command.End };

			Cube clipped;
			if (Intersect(c, region, &clipped))
			{
				const vector<Cube>& src = cubes[activeBuffer];
				vector<Cube>& dst = cubes[1 - activeBuffer];
				dst.clear();

				ShardExisting(clipped, src, &dst);
				if (command.On)
				{
					dst.push_back(clipped);
				}

				activeBuffer = 1 - activeBuffer;
			}
		}

		for (const Cube& c : cubes[activeBuffer])
		{
			int64_t volume = 1;
			volume *= c.End.X - c.Begin.X;
			volume *= c.End.Y - c.Begin.Y;
			volume *= c.End.Z - c.Begin.Z;
			assert(volume > 0);
			answer += volume;
		}
	}

	PuzzleOutput::Submit(2021, 22, 2, answer);
}

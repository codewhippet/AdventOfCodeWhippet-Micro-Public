#include "stdafx.h"

using namespace std;

namespace Puzzle17_2020_Types
{
	struct Neighbour
	{
		uint32_t Add = 0;
		uint32_t Sub = 0;
	};

	struct InactiveCountBuffer
	{
		InactiveCountBuffer(const Vec4Int& dimensions)
			: Data(dimensions.X * dimensions.Y * dimensions.Z * dimensions.W)
		{
		}

		void Increment(uint32_t pos, int8_t inc)
		{
			Data[pos] += inc;
		}

		void Extract(HashSet<uint32_t>* next, int8_t target)
		{
			for (uint32_t index = 0; index < static_cast<uint32_t>(Data.size()); index++)
			{
				int8_t count = Data[index];
				if (count == target)
				{
					next->Insert(index);
				}
			}
		}

		void Reset()
		{
			ranges::fill(Data, static_cast<int8_t>(0));
		}

		vector<int8_t> Data;
	};
}

using namespace Puzzle17_2020_Types;

static void ReadStartingConfiguration(HashSet<uint32_t>* config, const Vec4Int& offset, const Vec4Int& dimensions)
{
	for (int32_t y = 0; y < numeric_limits<int32_t>::max(); y++)
	{
		for (int32_t x = 0; x < numeric_limits<int32_t>::max(); x++)
		{
			int c = PuzzleInput::GetChar();
			if (c == EOF)
				return;

			if (c == '\n')
				break;

			if (c == '#')
			{
				uint32_t cell = 0;
				cell += offset.W;
				cell *= dimensions.Z;
				cell += offset.Z;
				cell *= dimensions.Y;
				cell += y + offset.Y;
				cell *= dimensions.X;
				cell += x + offset.X;

				config->Insert(cell);
			}
		}
	}
}

static vector<Neighbour> Generate3DNeighbours(const Vec4Int& dimensions)
{
	vector<Neighbour> neighbours;
	neighbours.reserve((3 * 3 * 3) - 1);

	for (int32_t z : { -1, 0, 1 })
	{
		for (int32_t y : { -1, 0, 1 })
		{
			for (int32_t x : { -1, 0, 1 })
			{
				if (Vec3Int{ x, y, z } == Vec3Int{})
					continue;

				Neighbour neighbour;

				if (x > 0) neighbour.Add += 1;
				if (x < 0) neighbour.Sub += 1;

				if (y > 0) neighbour.Add += dimensions.X;
				if (y < 0) neighbour.Sub += dimensions.X;

				if (z > 0) neighbour.Add += dimensions.X * dimensions.Y;
				if (z < 0) neighbour.Sub += dimensions.X * dimensions.Y;

				neighbours.push_back(neighbour);
			}
		}
	}
	return neighbours;
}

static vector<Neighbour> Generate4DNeighbours(const Vec4Int& dimensions)
{
	vector<Neighbour> neighbours;
	neighbours.reserve((3 * 3 * 3 * 3) - 1);

	for (int32_t w : { -1, 0, 1 })
	{
		for (int32_t z : { -1, 0, 1 })
		{
			for (int32_t y : { -1, 0, 1 })
			{
				for (int32_t x : { -1, 0, 1 })
				{
					if (Vec4Int{ x, y, z, w } == Vec4Int{})
						continue;

					Neighbour neighbour;

					if (x > 0) neighbour.Add += 1;
					if (x < 0) neighbour.Sub += 1;

					if (y > 0) neighbour.Add += dimensions.X;
					if (y < 0) neighbour.Sub += dimensions.X;

					if (z > 0) neighbour.Add += dimensions.X * dimensions.Y;
					if (z < 0) neighbour.Sub += dimensions.X * dimensions.Y;

					if (w > 0) neighbour.Add += dimensions.X * dimensions.Y * dimensions.Z;
					if (w < 0) neighbour.Sub += dimensions.X * dimensions.Y * dimensions.Z;

					neighbours.push_back(neighbour);
				}
			}
		}
	}
	return neighbours;
}

static void Step(const HashSet<uint32_t> &current, const vector<Neighbour> &offsets, InactiveCountBuffer* inactiveCounts, HashSet<uint32_t>* next)
{
	next->Reset();
	inactiveCounts->Reset();

	for (const uint32_t& cube : current)
	{
		inactiveCounts->Increment(cube, 10); // Make sure we don't re-activate ourselves

		int32_t neighbourCount = 0;
		for (const Neighbour& neighbourOffset : offsets)
		{
			uint32_t neighbour = cube + neighbourOffset.Add - neighbourOffset.Sub;
			if (current.Contains(neighbour))
			{
				neighbourCount++;
			}
			inactiveCounts->Increment(neighbour, 1);
		}
		if ((neighbourCount == 2) || (neighbourCount == 3))
		{
			next->Insert(cube);
		}
	}

	inactiveCounts->Extract(next, 3);
}

void Puzzle17_A_2020()
{
	array<HashSet<uint32_t>, 2> conway =
	{
		HashSet<uint32_t>{ 1024, 0 },
		HashSet<uint32_t>{ 1024, 0 }
	};

	const Vec4Int spatialDimensions{ 22, 22, 15, 1 };

	ReadStartingConfiguration(&conway[0], { 7, 7, 7, 0 }, spatialDimensions);

	InactiveCountBuffer inactiveCounts(spatialDimensions);

	vector<Neighbour> neighbours = Generate3DNeighbours(spatialDimensions);
	for (size_t i = 0; i < 6; i++)
	{
		Step(conway[i & 1], neighbours, &inactiveCounts, &conway[1 - (i & 1)]);
	}

	int32_t answer = conway[0].Size();

	PuzzleOutput::Submit(2020, 17, 1, answer);
}

void Puzzle17_B_2020()
{
	array<HashSet<uint32_t>, 2> conway =
	{
		HashSet<uint32_t>{ 4096, 0 },
		HashSet<uint32_t>{ 4096, 0 }
	};

	const Vec4Int spatialDimensions{ 22, 22, 15, 15 };

	ReadStartingConfiguration(&conway[0], { 7, 7, 7, 7 }, spatialDimensions);

	InactiveCountBuffer inactiveCounts(spatialDimensions);

	vector<Neighbour> neighbours = Generate4DNeighbours(spatialDimensions);
	for (size_t i = 0; i < 6; i++)
	{
		Step(conway[i & 1], neighbours, &inactiveCounts, &conway[1 - (i & 1)]);
	}

	int32_t answer = conway[0].Size();

	PuzzleOutput::Submit(2020, 17, 2, answer);
}

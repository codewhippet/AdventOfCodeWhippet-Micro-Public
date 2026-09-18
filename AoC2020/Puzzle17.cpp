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
			: Dimensions(dimensions)
		{
			Data.resize(Dimensions.X * Dimensions.Y * Dimensions.Z * Dimensions.W);
		}

		void Increment(uint32_t pos, int8_t inc)
		{
			Vec4Int decodedPos;
			decodedPos.X = (pos >> 24) & 0xff;
			decodedPos.Y = (pos >> 16) & 0xff;
			decodedPos.Z = (pos >> 8) & 0xff;
			decodedPos.W = (pos >> 0) & 0xff;
			decodedPos = decodedPos;

			assert((decodedPos.X >= 0) && (decodedPos.X < Dimensions.X));
			assert((decodedPos.Y >= 0) && (decodedPos.Y < Dimensions.Y));
			assert((decodedPos.Z >= 0) && (decodedPos.Z < Dimensions.Z));
			assert((decodedPos.W >= 0) && (decodedPos.W < Dimensions.W));

			size_t index =  0;
			index += decodedPos.W;
			index *= Dimensions.Z;
			index += decodedPos.Z;
			index *= Dimensions.Y;
			index += decodedPos.Y;
			index *= Dimensions.X;
			index += decodedPos.X;

			Data[index] += inc;
		}

		void Extract(HashSet<uint32_t>* next, int8_t target)
		{
			size_t index = 0;
			for (int32_t w = 0; w < Dimensions.W; w++)
			{
				for (int32_t z = 0; z < Dimensions.Z; z++)
				{
					for (int32_t y = 0; y < Dimensions.Y; y++)
					{
						for (int32_t x = 0; x < Dimensions.X; x++)
						{
							int8_t count = Data[index];
							if (count == target)
							{
								next->Insert((x << 24) | (y << 16) | (z << 8) | (w << 0));
							}

							index++;
						}
					}
				}
			}
		}

		void Reset()
		{
			ranges::fill(Data, static_cast<int8_t>(0));
		}

		Vec4Int Dimensions;
		vector<int8_t> Data;
	};
}

using namespace Puzzle17_2020_Types;

static void ReadStartingConfiguration(HashSet<uint32_t>* config, const Vec4Int& offset)
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
				cell += (offset.X + x) << 24;
				cell += (offset.Y + y) << 16;
				cell += (offset.Z    ) << 8;
				cell += (offset.W    ) << 0;
				config->Insert(cell);
			}
		}
	}
}

static vector<Neighbour> Generate3DNeighbours()
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

				if (x > 0) neighbour.Add |= 1 << 24;
				if (x < 0) neighbour.Sub |= 1 << 24;

				if (y > 0) neighbour.Add |= 1 << 16;
				if (y < 0) neighbour.Sub |= 1 << 16;

				if (z > 0) neighbour.Add |= 1 << 8;
				if (z < 0) neighbour.Sub |= 1 << 8;

				neighbours.push_back(neighbour);
			}
		}
	}
	return neighbours;
}

static vector<Neighbour> Generate4DNeighbours()
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

					if (x > 0) neighbour.Add |= 1 << 24;
					if (x < 0) neighbour.Sub |= 1 << 24;

					if (y > 0) neighbour.Add |= 1 << 16;
					if (y < 0) neighbour.Sub |= 1 << 16;

					if (z > 0) neighbour.Add |= 1 << 8;
					if (z < 0) neighbour.Sub |= 1 << 8;

					if (w > 0) neighbour.Add |= 1 << 0;
					if (w < 0) neighbour.Sub |= 1 << 0;

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

	ReadStartingConfiguration(&conway[0], { 7, 7, 7, 0 });

	InactiveCountBuffer inactiveCounts(Vec4Int{ 22, 22, 15, 1 });

	vector<Neighbour> neighbours = Generate3DNeighbours();
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

	ReadStartingConfiguration(&conway[0], { 7, 7, 7, 7 });

	InactiveCountBuffer inactiveCounts(Vec4Int{ 22, 22, 15, 15 });

	vector<Neighbour> neighbours = Generate4DNeighbours();
	for (size_t i = 0; i < 6; i++)
	{
		Step(conway[i & 1], neighbours, &inactiveCounts, &conway[1 - (i & 1)]);
	}

	int32_t answer = conway[0].Size();

	PuzzleOutput::Submit(2020, 17, 2, answer);
}

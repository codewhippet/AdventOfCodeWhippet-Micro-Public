#include "stdafx.h"

using namespace std;

namespace Puzzle22_2017_Types
{
	enum : uint8_t
	{
		CLEANED = 0,
		WEAKENED = 1,
		INFECTED = 2,
		FLAGGED = 3
	};

	struct CompressedMap
	{
		CompressedMap(const Vec2Int& offset, int32_t powerOfTwoSize);

		uint8_t Get(const Vec2Int& pos) const;
		void Set(const Vec2Int& pos, uint8_t state);

		uint8_t GetStatePostIncrement(const Vec2Int& pos);

		vector<uint8_t> Data;
		Vec2Int Offset;
		int32_t Stride;
	};
}

using namespace Puzzle22_2017_Types;

Puzzle22_2017_Types::CompressedMap::CompressedMap(const Vec2Int& offset, int32_t powerOfTwoSize)
	: Offset(offset)
{
	assert(powerOfTwoSize >= 2);
	int32_t size = 1 << powerOfTwoSize;
	Stride = size >> 2;

	Data.resize(size * Stride, CLEANED);
}

uint8_t CompressedMap::Get(const Vec2Int& pos) const
{
	Vec2Int relativePos = pos - Offset;

	int32_t dataIndex = relativePos.Y * Stride + (relativePos.X >> 2);
	int32_t dataShift = (relativePos.X & 0x3) << 1;

	return (Data[dataIndex] >> dataShift) & 0x3;
}

void Puzzle22_2017_Types::CompressedMap::Set(const Vec2Int& pos, uint8_t state)
{
	Vec2Int relativePos = pos - Offset;

	int32_t dataIndex = relativePos.Y * Stride + (relativePos.X >> 2);
	int32_t dataShift = (relativePos.X & 0x3) << 1;

	uint8_t existingData = Data[dataIndex];
	uint8_t newData = existingData & ~(0x3 << dataShift);
	newData |= state << dataShift;

	Data[dataIndex] = newData;
}

uint8_t Puzzle22_2017_Types::CompressedMap::GetStatePostIncrement(const Vec2Int& pos)
{
	Vec2Int relativePos = pos - Offset;

	int32_t dataIndex = relativePos.Y * Stride + (relativePos.X >> 2);
	int32_t dataShift = (relativePos.X & 0x3) << 1;

	uint8_t existingData = Data[dataIndex];

	uint8_t currentState = (existingData >> dataShift) & 0x3;
	uint8_t nextState = (currentState + 1) & 0x3;

	uint8_t newData = existingData & ~(0x3 << dataShift);
	newData |= nextState << dataShift;

	Data[dataIndex] = newData;

	return currentState;
}

static CompressedMap ReadCompressedMap(Vec2Int* startingSize)
{
	CompressedMap network({ -256, -256 }, 9); // 512*512 / 4 = 64KiB

	Vec2Int readPos;
	Vec2Int maxSize;
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case '#':
			network.Set(readPos, INFECTED);
			readPos.X++;
			break;
		case '.':
			readPos.X++;
			break;
		case '\n':
			readPos.X = 0;
			readPos.Y++;
			break;
		default:
			assert(false);
		}

		maxSize = Vec2Int::MaxElements(maxSize, readPos);
	}

	*startingSize = maxSize;
	return network;
}

void Puzzle22_A_2017()
{
	Vec2Int startingSize;
	CompressedMap network = ReadCompressedMap(&startingSize);

	Vec2Int virusPos{ startingSize.X / 2, startingSize.Y / 2 };
	Vec2Int virusDir = Vec2Int::Up();

	int32_t answer = 0;
	for (size_t i = 0; i < 10000; i++)
	{
		uint8_t currentState = network.Get(virusPos);
		if (currentState == INFECTED)
		{
			virusDir = Vec2Int::RotateClockwise(virusDir);
			network.Set(virusPos, CLEANED);
		}
		else
		{
			virusDir = Vec2Int::RotateAnticlockwise(virusDir);
			network.Set(virusPos, INFECTED);
			answer++;
		}

		virusPos = virusPos + virusDir;
	}

	PuzzleOutput::Submit(2017, 22, 1, answer);
}

void Puzzle22_B_2017()
{
	Vec2Int startingSize;
	CompressedMap network = ReadCompressedMap(&startingSize);

	Vec2Int virusPos{ startingSize.X / 2, startingSize.Y / 2 };
	Vec2Int virusDir = Vec2Int::Up();

	int32_t answer = 0;
	for (size_t i = 0; i < 10000000; i++)
	{
		uint8_t currentState = network.GetStatePostIncrement(virusPos);
		switch (currentState)
		{
		case CLEANED:
			virusDir = Vec2Int::RotateAnticlockwise(virusDir);
			break;
		case WEAKENED:
			answer++;
			break;
		case INFECTED:
			virusDir = Vec2Int::RotateClockwise(virusDir);
			break;
		case FLAGGED:
			virusDir = Vec2Int::RotateClockwise(Vec2Int::RotateClockwise(virusDir));
			break;
		}

		virusPos = virusPos + virusDir;
	}

	PuzzleOutput::Submit(2017, 22, 2, answer);
}

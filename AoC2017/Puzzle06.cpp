#include "stdafx.h"

using namespace std;

namespace Puzzle06_2017_Types
{
	enum : size_t
	{
		NUM_BANKS = 16,
		BANK_SIZE_MASK = NUM_BANKS - 1,
	};

	struct Memory
	{
		uint8_t Bank[NUM_BANKS];
		auto operator<=>(const Memory&) const = default;
	};
}

using namespace Puzzle06_2017_Types;

template <>
struct std::hash<Memory>
{
	size_t operator()(const Memory& m) const noexcept
	{
		size_t hash = 0;
		for (size_t i = 0; i < NUM_BANKS; i++)
		{
			hash = (hash << 2) ^ std::hash<uint8_t>{}(m.Bank[i]);
		}
		return hash;
	}
};

static Memory ReadStartingMemory()
{
	Memory m;
	for (size_t i = 0; i < NUM_BANKS; i++)
	{
		m.Bank[i] = static_cast<uint8_t>(Parse::GetUint32());
	}
	return m;
}

static size_t FindLargestBucketIndex(const Memory& buckets)
{
	size_t largestElementIndex = 0;
	for (size_t i = 1; i < NUM_BANKS; i++)
	{
		if (buckets.Bank[i] > buckets.Bank[largestElementIndex])
		{
			largestElementIndex = i;
		}
	}
	return largestElementIndex;
}

static void DistributeLargestBucket(Memory* buckets)
{
	size_t largestBucket = FindLargestBucketIndex(*buckets);
	uint8_t distribute = buckets->Bank[largestBucket];
	buckets->Bank[largestBucket] = 0;
	for (size_t sprinkle = 0; sprinkle < distribute; sprinkle++)
	{
		buckets->Bank[(largestBucket + sprinkle + 1) & BANK_SIZE_MASK]++;
	}
}

void Puzzle06_A_2017()
{
	Memory mem = ReadStartingMemory();

	int32_t answer = 0;

	HashSet<Memory> states(8 * 1024, { 0xff }); // 8 * 1024 * 16 bytes = 128kb
	while (true)
	{
		answer++;

		DistributeLargestBucket(&mem);
		if (states.Insert(mem) == false)
		{
			break;
		}
	}

	PuzzleOutput::Submit(2017, 6, 1, answer);
}

void Puzzle06_B_2017()
{
	Memory mem = ReadStartingMemory();

	int32_t answer = 0;

	uint16_t steps = 0;

	HashMap<Memory, uint16_t> states(8 * 1024, { 0xff }); // 8 * 1024 * 18 bytes = 144kb
	while (true)
	{
		steps++;

		DistributeLargestBucket(&mem);
		uint16_t stepsFromPreviousState;
		if (states.TryFind(mem, &stepsFromPreviousState))
		{
			answer = steps - stepsFromPreviousState;
			break;
		}

		states.Insert(mem, steps);
	}

	PuzzleOutput::Submit(2017, 6, 2, answer);
}

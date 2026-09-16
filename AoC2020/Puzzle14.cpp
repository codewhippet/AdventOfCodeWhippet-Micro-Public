#include "stdafx.h"

using namespace std;

namespace Puzzle14_2020_Types
{
	struct Mask
	{
		uint64_t AND = ~0ull;
		uint64_t OR = 0ull;
	};

	struct FloatingMask
	{
		uint64_t Ones;
		uint64_t Zeros;
		uint64_t Floating;
	};

	struct AddressRange
	{
		uint64_t Address = 0ull;
		uint64_t Floating = 0ull;
	};
}

using namespace Puzzle14_2020_Types;

static Mask MakeMask()
{
	Mask m;

	for (int c : Parse::ReadUntilSeen('\n'))
	{
		m.AND = m.AND << 1;
		m.OR = m.OR << 1;

		switch (c)
		{
		case '0':
			// Keep AND as 0
			break;

		case '1':
			m.AND |= 1ll;
			m.OR |= 1ll;
			break;

		case 'X':
			m.AND |= 1ll;
			break;
		}
	}

	return m;
}

static FloatingMask MakeFloatingMask()
{
	FloatingMask m{};
	for (int c : Parse::ReadUntilSeen('\n'))
	{
		m.Ones <<= 1;
		m.Zeros <<= 1;
		m.Floating <<= 1;

		switch (c)
		{
		case '0':
			m.Zeros |= 1;
			break;

		case '1':
			m.Ones |= 1;
			break;

		case 'X':
			m.Floating |= 1;
			break;
		}
	}
	return m;
}

static bool DoesIntersect(const AddressRange& a, const AddressRange& b)
{
	uint64_t fixedBits = ~(a.Floating | b.Floating);
	return (a.Address & fixedBits) == (b.Address & fixedBits);
}

static AddressRange Intersect(const AddressRange& a, const AddressRange& b)
{
	uint64_t aFixed = a.Address & ~a.Floating;
	uint64_t bFixed = b.Address & ~b.Floating;
	return { aFixed | bFixed, a.Floating & b.Floating };
}

static uint64_t SizeOfRange(const AddressRange& a)
{
	return 1ull << popcount(a.Floating);
}

void Puzzle14_A_2020()
{
	HashMap<uint64_t, uint64_t> memory(1024, numeric_limits<uint64_t>::max());

	Mask mask{};
	while (PuzzleInput::NextLine())
	{
		PuzzleInput::DropChar();
		switch (PuzzleInput::GetChar())
		{
		case 'a':
			{
				Parse::DiscardExpected("sk = ");
				mask = MakeMask();
			}
			break;

		case 'e':
			{
				uint64_t address = Parse::GetUint64();
				uint64_t unmaskedValue = Parse::GetUint64();
				uint64_t maskedValue = (unmaskedValue & mask.AND) | mask.OR;
				memory[address] = maskedValue;
			}
			break;
		}
	}

	int64_t answer = 0;
	for (const auto& data : memory)
	{
		answer += data.second;
	}

	PuzzleOutput::Submit(2020, 14, 1, answer);
}

void Puzzle14_B_2020()
{
	vector<pair<AddressRange, uint64_t>> operations;
	operations.reserve(600);

	FloatingMask mask{};
	while (PuzzleInput::NextLine())
	{
		PuzzleInput::DropChar();
		switch (PuzzleInput::GetChar())
		{
		case 'a':
			{
				Parse::DiscardExpected("sk = ");
				mask = MakeFloatingMask();
			}
			break;

		case 'e':
			{
				uint64_t unmaskedAddress = Parse::GetUint64();
				uint64_t maskedAddress = (unmaskedAddress & mask.Zeros) | mask.Ones;
				uint64_t value = Parse::GetUint64();
				operations.push_back({ { maskedAddress, mask.Floating }, value });
			}
			break;
		}
	}

	vector<AddressRange> intersections;
	intersections.reserve(8);

	uint64_t total = 0;
	for (size_t i = 0; i < operations.size(); i++)
	{
		const AddressRange& aAddr = operations[i].first;
		uint64_t value = operations[i].second;

		intersections.clear();
		for (size_t j = i + 1; j < operations.size(); j++)
		{
			const AddressRange& bAddr = operations[j].first;
			if (DoesIntersect(aAddr, bAddr))
			{
				// Add A n B to the starting sets to check
				intersections.push_back(Intersect(aAddr, bAddr));
			}
		}

		uint64_t rangeSize = SizeOfRange(aAddr);

		if (intersections.size() == 1)
		{
			rangeSize -= SizeOfRange(intersections[0]);
		}
		else
		{
			for (uint32_t combination = 1; combination < (1u << intersections.size()); combination++)
			{
				bool validSubset = true;

				AddressRange subset = aAddr;
				for (int32_t setIndex = 0; setIndex < static_cast<int32_t>(intersections.size()); setIndex++)
				{
					if (combination & (1 << setIndex))
					{
						if (DoesIntersect(subset, intersections[setIndex]))
						{
							subset = Intersect(subset, intersections[setIndex]);
						}
						else
						{
							validSubset = false;
							break;
						}
					}
				}

				if (validSubset)
				{
					int64_t sign = popcount(combination) & 1 ? -1 : 1;
					rangeSize += SizeOfRange(subset) * sign;
				}
			}
		}

		total += rangeSize * value;
	}

	int64_t answer = total;
	PuzzleOutput::Submit(2020, 14, 2, answer);
}

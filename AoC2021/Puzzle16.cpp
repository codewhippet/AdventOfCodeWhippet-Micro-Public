#include "stdafx.h"

using namespace std;

namespace Puzzle16_2021_Types
{
	struct BitSource
	{
		uint32_t Chunk = 0;
		uint32_t AvailableBits = 0;
		uint32_t BitsServed = 0;
		vector<uint32_t> HexToNibble;

		BitSource()
			: HexToNibble(128)
		{
			HexToNibble['0'] = 0;
			HexToNibble['1'] = 1;
			HexToNibble['2'] = 2;
			HexToNibble['3'] = 3;
			HexToNibble['4'] = 4;
			HexToNibble['5'] = 5;
			HexToNibble['6'] = 6;
			HexToNibble['7'] = 7;
			HexToNibble['8'] = 8;
			HexToNibble['9'] = 9;
			HexToNibble['A'] = 10;
			HexToNibble['B'] = 11;
			HexToNibble['C'] = 12;
			HexToNibble['D'] = 13;
			HexToNibble['E'] = 14;
			HexToNibble['F'] = 15;
		}

		uint32_t GetBits(uint32_t numBits)
		{
			assert(numBits > 0 && numBits <= 32);

			while (AvailableBits < numBits)
				ReadNibble();

			uint32_t bits = Chunk >> (32 - numBits);
			Chunk <<= numBits;
			AvailableBits -= numBits;
			BitsServed += numBits;

			return bits;
		}

		void ReadNibble()
		{
			assert(AvailableBits <= 28);
			uint32_t nibble = HexToNibble[PuzzleInput::GetChar()];
			Chunk |= nibble << (32 - AvailableBits - 4);
			AvailableBits += 4;
		}
	};

	struct ExecState
	{
		uint32_t Type;
		uint32_t LengthType;
		uint32_t SubpacketBitsEnd;
		uint32_t SubpacketCount;
		uint32_t SubvaluesBegin;
	};
}

using namespace Puzzle16_2021_Types;

static uint32_t VersionSum(BitSource* bits)
{
	const size_t execStackSize = 32;

	uint32_t versionSum = 0;

	vector<ExecState> exec;
	exec.reserve(execStackSize);

	auto pushNewState = [&]()
		{
			ExecState state;
			state.Type = numeric_limits<uint32_t>::max();
			state.LengthType = 0;
			state.SubpacketBitsEnd = 0;
			state.SubpacketCount = 0;
			state.SubvaluesBegin = numeric_limits<uint32_t>::max();
			exec.push_back(state);
		};

	pushNewState();
	while (exec.size() > 0)
	{
		ExecState& current = exec.back();
		if (current.SubpacketBitsEnd && (bits->BitsServed < current.SubpacketBitsEnd))
		{
			pushNewState();
			continue;
		}
		else if (current.SubpacketCount)
		{
			current.SubpacketCount--;
			pushNewState();
			continue;
		}

		switch (current.Type)
		{
		case numeric_limits<uint32_t>::max():
			{
				uint32_t version = bits->GetBits(3);
				versionSum += version;

				current.Type = bits->GetBits(3);
				if (current.Type == 4)
					break;

				current.LengthType = bits->GetBits(1);
				if (current.LengthType == 0)
				{
					uint32_t subPacketBits = bits->GetBits(15);
					uint32_t subPacketStart = bits->BitsServed;
					current.SubpacketBitsEnd = subPacketStart + subPacketBits;
				}
				else
				{
					current.SubpacketCount = bits->GetBits(11);
				}
			}
			break;

		case 4:
			{
				while (true)
				{
					uint32_t nibble = bits->GetBits(5);
					bool isLastNibble = nibble < 16;
					if (isLastNibble)
						break;
				}
				exec.pop_back();
			}
			break;

		case 0:
		case 1:
		case 2:
		case 3:
		case 5:
		case 6:
		case 7:
			{
				exec.pop_back();
			}
			break;
		}
	}

	return versionSum;
}

static uint64_t Value(BitSource* bits)
{
	const size_t execStackSize = 32;
	const size_t valuesStackSize = 64;

	vector<ExecState> exec;
	exec.reserve(execStackSize);
	vector<uint64_t> values;
	values.reserve(valuesStackSize);

	auto pushNewState = [&]()
		{
			ExecState state;
			state.Type = numeric_limits<uint32_t>::max();
			state.LengthType = 0;
			state.SubpacketBitsEnd = 0;
			state.SubpacketCount = 0;
			state.SubvaluesBegin = static_cast<uint32_t>(values.size());
			exec.push_back(state);
		};

	pushNewState();
	while (exec.size() > 0)
	{
		ExecState& current = exec.back();
		if (current.SubpacketBitsEnd && (bits->BitsServed < current.SubpacketBitsEnd))
		{
			pushNewState();
			continue;
		}
		else if (current.SubpacketCount)
		{
			current.SubpacketCount--;
			pushNewState();
			continue;
		}

		switch (current.Type)
		{
		case numeric_limits<uint32_t>::max():
			{
				uint32_t version = bits->GetBits(3);
				(void)version;

				current.Type = bits->GetBits(3);
				if (current.Type == 4)
					break;

				current.LengthType = bits->GetBits(1);
				if (current.LengthType == 0)
				{
					uint32_t subPacketBits = bits->GetBits(15);
					uint32_t subPacketStart = bits->BitsServed;
					current.SubpacketBitsEnd = subPacketStart + subPacketBits;
				}
				else
				{
					current.SubpacketCount = bits->GetBits(11);
				}
			}
			break;

		case 0:
			{
				uint64_t sum = accumulate(values.begin() + current.SubvaluesBegin, values.end(), static_cast<uint64_t>(0), plus<uint64_t>());
				values.resize(current.SubvaluesBegin);
				values.push_back(sum);
				exec.pop_back();
			}
			break;

		case 1:
			{
				uint64_t product = accumulate(values.begin() + current.SubvaluesBegin, values.end(), static_cast<uint64_t>(1), multiplies<uint64_t>());
				values.resize(current.SubvaluesBegin);
				values.push_back(product);
				exec.pop_back();
			}
			break;

		case 2:
			{
				uint64_t smallest = *min_element(values.begin() + current.SubvaluesBegin, values.end());
				values.resize(current.SubvaluesBegin);
				values.push_back(smallest);
				exec.pop_back();
			}
			break;

		case 3:
			{
				uint64_t largest = *max_element(values.begin() + current.SubvaluesBegin, values.end());
				values.resize(current.SubvaluesBegin);
				values.push_back(largest);
				exec.pop_back();
			}
			break;

		case 4:
			{
				uint64_t value = 0;
				while (true)
				{
					value <<= 4;

					uint32_t nibble = bits->GetBits(5);
					value += nibble & 0xf;

					bool isLastNibble = nibble < 16;
					if (isLastNibble)
						break;
				}
				values.push_back(value);
				exec.pop_back();
			}
			break;

		case 5:
			{
				assert(values.size() == current.SubvaluesBegin + 2);
				uint64_t result = values[current.SubvaluesBegin] > values[current.SubvaluesBegin + 1] ? 1 : 0;
				values.resize(current.SubvaluesBegin);
				values.push_back(result);
				exec.pop_back();
			}
			break;

		case 6:
			{
				assert(values.size() == current.SubvaluesBegin + 2);
				uint64_t result = values[current.SubvaluesBegin] < values[current.SubvaluesBegin + 1] ? 1 : 0;
				values.resize(current.SubvaluesBegin);
				values.push_back(result);
				exec.pop_back();
			}
			break;

		case 7:
			{
				assert(values.size() == current.SubvaluesBegin + 2);
				uint64_t result = values[current.SubvaluesBegin] == values[current.SubvaluesBegin + 1] ? 1 : 0;
				values.resize(current.SubvaluesBegin);
				values.push_back(result);
				exec.pop_back();
			}
			break;
		}
	}

	assert(values.size() == 1);
	return values[0];
}

void Puzzle16_A_2021()
{
	BitSource bits;
	int32_t answer = VersionSum(&bits);

	PuzzleOutput::Submit(2021, 16, 1, answer);
}

void Puzzle16_B_2021()
{
	BitSource bits;
	int64_t answer = Value(&bits);

	PuzzleOutput::Submit(2021, 16, 2, answer);
}

#include "stdafx.h"

using namespace std;

namespace Puzzle03_2021_Types
{
}

using namespace Puzzle03_2021_Types;

static pair<int32_t, int32_t> CountZerosAndOnes(const ranges::subrange<vector<uint32_t>::iterator>& lines, uint32_t testBit)
{
	int32_t oneCount = 0;
	for (uint32_t line : lines)
	{
		oneCount += (line & testBit ? 1 : 0);
	}
	int32_t zeroCount = static_cast<int>(lines.size()) - oneCount;

	return make_pair(zeroCount, oneCount);
}

void Puzzle03_A_2021()
{
	const size_t lineWidth = 12;

	int32_t lineCount = 0;
	array<int32_t, lineWidth> oneCounts{};
	while (PuzzleInput::NextLine())
	{
		for (size_t i = 0; i < lineWidth; i++)
		{
			oneCounts[i] += PuzzleInput::GetChar() == '1';
		}

		lineCount++;
	}

	int32_t gamma = 0;
	for (size_t i = 0; i < lineWidth; i++)
	{
		gamma <<= 1;
		if (oneCounts[i] > lineCount / 2)
		{
			gamma |= 1;
		}
	}

	int32_t epsilon = (1 << lineWidth) - 1;
	epsilon ^= gamma;

	int32_t answer = gamma * epsilon;

	PuzzleOutput::Submit(2021, 3, 1, answer);
}

void Puzzle03_B_2021()
{
	const size_t lineWidth = 12;
	const size_t expectedNumLines = 1000;

	vector<uint32_t> lines;
	lines.reserve(expectedNumLines);

	while (PuzzleInput::NextLine())
	{
		uint32_t line = 0;
		for (size_t i = 0; i < lineWidth; i++)
		{
			line = (line << 1) | (PuzzleInput::GetChar() == '1');
		}
		lines.push_back(line);
	}

	uint32_t oxygenGenerator;
	{
		uint32_t testBit = 1 << (lineWidth - 1);

		vector<uint32_t>::iterator oxygenBegin = lines.begin();
		vector<uint32_t>::iterator oxygenEnd = lines.end();
		while (distance(oxygenBegin, oxygenEnd) > 1)
		{
			pair<int32_t, int32_t> counts = CountZerosAndOnes(ranges::subrange{ oxygenBegin, oxygenEnd }, testBit);

			uint32_t keep = counts.second >= counts.first ? testBit : 0;

			oxygenEnd = partition(oxygenBegin, oxygenEnd,
				[&](uint32_t l)
				{
					return (l & testBit) == keep;
				});

			testBit >>= 1;
		}

		oxygenGenerator = *oxygenBegin;
	}

	uint32_t co2Scrubber;
	{
		uint32_t testBit = 1 << (lineWidth - 1);

		vector<uint32_t>::iterator co2Begin = lines.begin();
		vector<uint32_t>::iterator co2End = lines.end();
		while (distance(co2Begin, co2End) > 1)
		{
			pair<int32_t, int32_t> counts = CountZerosAndOnes(ranges::subrange{ co2Begin, co2End }, testBit);

			uint32_t keep = counts.first <= counts.second ? 0 : testBit;

			co2End = partition(co2Begin, co2End,
				[&](uint32_t l)
				{
					return (l & testBit) == keep;
				});

			testBit >>= 1;
		}

		co2Scrubber = *co2Begin;
	}

	int32_t answer = oxygenGenerator * co2Scrubber;

	PuzzleOutput::Submit(2021, 3, 2, answer);
}

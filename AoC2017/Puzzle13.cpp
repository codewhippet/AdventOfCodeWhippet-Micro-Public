#include "stdafx.h"

using namespace std;

namespace Puzzle13_2017_Types
{
	struct Scanner
	{
		int32_t Offset;
		int32_t Period;
	};
}

using namespace Puzzle13_2017_Types;

static int32_t Sieve(const vector<Scanner>& scanners)
{
	vector<int8_t> caught(128 * 1024, 0);
	int32_t sieveOffset = 0;

	while (true)
	{
		for (const Scanner& scanner : scanners)
		{
			int32_t period = scanner.Period;
			int32_t chunkStartOffset = ((sieveOffset / period) * period) - sieveOffset - scanner.Offset;

			int32_t catchOn = chunkStartOffset;
			while (catchOn < 0)
				catchOn += period;

			while (catchOn < (int32_t)caught.size())
			{
				caught[catchOn] = 1;
				catchOn += period;
			}
		}

		auto findSlot = ranges::find(caught, 0);
		if (findSlot != caught.end())
		{
			return static_cast<int32_t>(distance(caught.begin(), findSlot) + sieveOffset);
		}

		sieveOffset += static_cast<int32_t>(caught.size());
		memset(caught.data(), 0, caught.size());
	}

	return -1;
}

void Puzzle13_A_2017()
{
	vector<Scanner> scanners;
	scanners.reserve(128);

	while (PuzzleInput::NextLine())
	{
		Scanner scanner;
		scanner.Offset = Parse::GetInt32();
		scanner.Period = (Parse::GetInt32() - 1) * 2;
		scanners.push_back(scanner);
	}

	auto severity = scanners | views::transform(
		[&](const auto& scanner)
		{
			return (scanner.Offset % scanner.Period == 0 ? scanner.Offset * ((scanner.Period / 2) + 1) : 0);
		});

	int32_t answer = accumulate(severity.begin(), severity.end(), 0);
	PuzzleOutput::Submit(2017, 13, 1, answer);
}

void Puzzle13_B_2017()
{
	vector<Scanner> scanners;
	scanners.reserve(128);

	while (PuzzleInput::NextLine())
	{
		Scanner scanner;
		scanner.Offset = Parse::GetInt32();
		scanner.Period = (Parse::GetInt32() - 1) * 2;
		scanners.push_back(scanner);
	}

	int32_t answer = Sieve(scanners);
	PuzzleOutput::Submit(2017, 13, 2, answer);
}

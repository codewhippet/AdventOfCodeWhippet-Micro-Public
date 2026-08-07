#include "stdafx.h"

using namespace std;

namespace Puzzle04_2015_Types
{
}

using namespace Puzzle04_2015_Types;

void IncrementDigits(char* digits, char* end)
{
	char incremented = ((*digits) += 1);
	if (incremented <= '9')
	{
		return;
	}
	(*digits--) -= 10;

	while (digits != end)
	{
		incremented = ++(*digits);
		if (incremented <= '9')
		{
			return;
		}
		*digits-- = '0';
	}
}

template <uint32_t ZERO_MASK>
int32_t FindLeadingZeros()
{
	uMD5::Chunk secretKey = {};

	int32_t secretLength = Parse::ReadLine(secretKey.chars, sizeof(secretKey));
	char* digits = &secretKey.chars[secretLength];
	char* secretEnd = digits - 1;

	const array<pair<size_t, int32_t>, 7> digitCounts =
	{
		pair<size_t, int32_t>{ 1, 10 },
		pair<size_t, int32_t>{ 2, 100 },
		pair<size_t, int32_t>{ 3, 1000 },
		pair<size_t, int32_t>{ 4, 10000 },
		pair<size_t, int32_t>{ 5, 100000 },
		pair<size_t, int32_t>{ 6, 1000000 },
		pair<size_t, int32_t>{ 7, 10000000 },
	};

	int32_t answer = 0;
	for (const auto& digitCount : digitCounts)
	{
		size_t digitsLength = digitCount.first;
		int32_t digitsMax = digitCount.second;

		sprint_digits(digits, answer, digitsLength);
		char* lastDigit = digits + digitsLength - 1;
		for (/***/; answer < digitsMax; answer += 1)
		{
			uint32_t hashPrefix = uMD5::HashMicroChunkInPlaceMSBOnly(&secretKey, secretLength + digitsLength);
			if ((hashPrefix & ZERO_MASK) == 0)
			{
				return answer;
			}

			IncrementDigits(lastDigit, secretEnd);
		}
	}

	return -1;
}

void Puzzle04_A_2015()
{
	int32_t answer = FindLeadingZeros<0x00f0ffff>();
	return PuzzleOutput::Submit(2015, 4, 1, answer);
}

void Puzzle04_B_2015()
{
	int32_t answer = FindLeadingZeros<0x00ffffff>();
	return PuzzleOutput::Submit(2015, 4, 2, answer);
}

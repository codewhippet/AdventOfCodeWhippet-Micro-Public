#include "stdafx.h"
#include <charconv>

using namespace std;

namespace Puzzle05_2016_Types
{
}

using namespace Puzzle05_2016_Types;

void Puzzle05_A_2016()
{
	uMD5::Chunk secretKey = {};
	int32_t secretLength = Parse::ReadLine(secretKey.chars, sizeof(secretKey));
	char* digits = &secretKey.chars[secretLength];

	const char hexDigits[] = "0123456789abcdef";

	char answer[9] = "........";

	int32_t counter = 0;
	for (size_t answerIndex = 0; answerIndex < 8; counter++)
	{
		int digitsLength = sprint_digits(digits, counter);

		uint32_t hashPrefix = uMD5::HashMiniChunkInPlaceMSBOnly(&secretKey, secretLength + digitsLength);
		if ((hashPrefix & 0x00f0ffff) == 0)
		{
			answer[answerIndex++] = hexDigits[(hashPrefix >> 16) & 0xf];
		}
	}

	return PuzzleOutput::Submit(2016, 5, 1, answer);
}

void Puzzle05_B_2016()
{
	uMD5::Chunk secretKey = {};
	int32_t secretLength = Parse::ReadLine(secretKey.chars, sizeof(secretKey));
	char* digits = &secretKey.chars[secretLength];

	const char hexDigits[] = "0123456789abcdef";

	char answer[9] = "........";

	int32_t counter = 0;
	for (size_t answerCharsFound = 0; answerCharsFound < 8; counter++)
	{
		int digitsLength = sprint_digits(digits, counter);

		uint32_t hashPrefix = uMD5::HashMiniChunkInPlaceMSBOnly(&secretKey, secretLength + digitsLength);
		if ((hashPrefix & 0x00f0ffff) == 0)
		{
			size_t index = (hashPrefix >> 16) & 0xf;
			if ((index < 8) && (answer[index] == '.'))
			{
				answer[index] = hexDigits[(hashPrefix >> 28) & 0xf];
				answerCharsFound++;
			}
		}
	}

	return PuzzleOutput::Submit(2016, 5, 2, answer);
}

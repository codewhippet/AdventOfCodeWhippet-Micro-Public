#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle04_2019_Types
{
}

using namespace Puzzle04_2019_Types;

static void IncrementDigits(char* digits, char* end)
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

static bool IsMonotonic(const char* digits, size_t numDigits)
{
	for (size_t i = 1; i < numDigits; i++)
	{
		if (digits[i - 1] > digits[i])
			return false;
	}

	return true;
}

static bool HasDouble(const char* digits, size_t numDigits)
{
	for (size_t i = 1; i < numDigits; i++)
	{
		if (digits[i - 1] == digits[i])
			return true;
	}

	return false;
}

static bool HasIsolatedDouble(const char* digits, size_t numDigits)
{
	for (size_t i = 1; i < numDigits; i++)
	{
		if (digits[i - 2] != digits[i - 1] &&
			digits[i - 1] == digits[i] &&
			digits[i] != digits[i + 1])
		{
			return true;
		}
	}

	return false;
}

void Puzzle04_A_2019()
{
	const int32_t from = Parse::GetInt32();
	PuzzleInput::DropChar();
	const int32_t to = Parse::GetInt32();

	assert(from >= 100000);
	assert(to <= 999999);
	const size_t numDigits = 6;

	array<char, numDigits + 2> digitsBuffer = {};
	char* digits = &digitsBuffer[1];

	sprint_digits(digits, from, numDigits);

	int32_t answer = 0;
	for (int32_t i = from; i <= to; i++)
	{
		if (IsMonotonic(digits, numDigits) && HasDouble(digits, numDigits))
		{
			answer++;
		}

		IncrementDigits(&digitsBuffer[numDigits], &digitsBuffer[0]);
	}

	PuzzleOutput::Submit(2019, 4, 1, answer);
}

void Puzzle04_B_2019()
{
	const int32_t from = Parse::GetInt32();
	PuzzleInput::DropChar();
	const int32_t to = Parse::GetInt32();

	assert(from >= 100000);
	assert(to <= 999999);
	const size_t numDigits = 6;

	array<char, numDigits + 2> digitsBuffer = {};
	char* digits = &digitsBuffer[1];

	sprint_digits(digits, from, numDigits);

	int32_t answer = 0;
	for (int32_t i = from; i <= to; i++)
	{
		if (IsMonotonic(digits, numDigits) && HasIsolatedDouble(digits, numDigits))
		{
			answer++;
		}

		IncrementDigits(&digitsBuffer[numDigits], &digitsBuffer[0]);
	}

	PuzzleOutput::Submit(2019, 4, 2, answer);
}

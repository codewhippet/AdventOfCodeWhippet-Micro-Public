#include "stdafx.h"

using namespace std;

namespace Puzzle11_2015_Types
{
}

using namespace Puzzle11_2015_Types;

static string_view Digits = "abcdefghjkmnpqrstuvwxyz";

static vector<uint8_t> DecodeDigits(const string_view& encoded)
{
	vector<uint8_t> digits;

	ranges::copy(encoded
		| views::transform([](char c) -> uint8_t
			{
				return (uint8_t)distance(Digits.begin(), find(Digits.begin(), Digits.end(), c));
			}),
		back_inserter(digits));
	ranges::reverse(digits);

	return digits;
}

static string EncodeDigits(const vector<uint8_t>& decoded)
{
	string digits;

	ranges::copy(decoded
		| views::transform([](uint8_t i)
			{
				return Digits[i];
			}),
		back_inserter(digits));
	ranges::reverse(digits);

	return digits;
}

static void Increment(vector<uint8_t>* digits)
{
	size_t i = 0;
	uint8_t carry = 1;
	while (carry > 0)
	{
		assert(i < digits->size());

		uint8_t incremented = (*digits)[i] + carry;
		carry = (uint8_t)(incremented / Digits.size());
		(*digits)[i] = incremented % Digits.size();

		i++;
	}
}

static bool IsValid(const vector<uint8_t>& digits)
{
	int decreaseRun = 0;
	bool hasDecrease = false;

	int pairCount = 0;
	bool suppressPairCheck = false;

	for (size_t i = 1; i < digits.size(); i++)
	{
		bool decrease = ((digits[i] - digits[i - 1]) == -1);
		decreaseRun = (decrease ? decreaseRun + 1 : 0);
		hasDecrease = hasDecrease || decreaseRun >= 2;

		bool same = (digits[i] == digits[i - 1]);
		if (same && !suppressPairCheck)
		{
			pairCount++;
			suppressPairCheck = true;
		}
		else
		{
			suppressPairCheck = false;
		}
	}

	return hasDecrease && pairCount >= 2;
}

void Puzzle11_A_2015()
{
	char line[32];
	Parse::ReadLine(line, sizeof(line));
	string_view password(line);

	vector<uint8_t> digits = DecodeDigits(password);
	do
	{
		Increment(&digits);

	} while (IsValid(digits) == false);

	string answer = EncodeDigits(digits);

	return PuzzleOutput::Submit(2015, 11, 1, answer.c_str());
}

void Puzzle11_B_2015()
{
	char line[32];
	Parse::ReadLine(line, sizeof(line));
	string_view password(line);

	vector<uint8_t> digits = DecodeDigits(password);
	for (int i = 0; i < 2; i++)
	{
		do
		{
			Increment(&digits);

		} while (IsValid(digits) == false);
	}

	string answer = EncodeDigits(digits);

	return PuzzleOutput::Submit(2015, 11, 2, answer.c_str());
}

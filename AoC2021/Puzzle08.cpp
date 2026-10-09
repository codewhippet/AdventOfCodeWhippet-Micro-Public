#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle08_2021_Types
{
}

using namespace Puzzle08_2021_Types;

// 0: ABC_EFG
// 1: __C__F_
// 2: A_CDE_G
// 3: A_CD_FG
// 4: _BCD_F_
// 5: AB_D_FG
// 6: AB_DEFG
// 7: A_C__F_
// 8: ABCDEFG
// 9: ABCD_FG

// Know: '1', '4', '7', '8'
// 
// A appears 8
// B appears 6
// C appears 8
// D appears 7
// E appears 4
// F appears 9
// G appears 7
// Therefore know B, E, F

// A = '7' - '1'
// B = Known
// C = '1' - F
// D = '4' - B - C - F
// E = Known
// F = Known
// G = '8' - A - B - C - D - E - F

static uint32_t ToDigits(const string_view& str)
{
	uint32_t digits = 0;
	for (char c : str)
	{
		digits |= 1u << (c - 'a');
	}
	return digits;
}

static void Solve(const vector<char>& digits, vector<int32_t>* mapping)
{
	vector<int32_t> charCounts(26);

	uint32_t D_1 = 0;
	uint32_t D_4 = 0;
	uint32_t D_7 = 0;
	uint32_t D_8 = 0;
	{
		size_t length = 0;
		for (size_t i = 0; i < digits.size(); i++)
		{
			char c = digits[i];
			if (c == ' ')
			{
				switch (length)
				{
				case 2: // '1' has 2 segments
					D_1 = ToDigits(string_view{ &digits[i - length], length });
					break;
				case 4: // '4' has 4 segments
					D_4 = ToDigits(string_view{ &digits[i - length], length });
					break;
				case 3: // '7' has 3 segments
					D_7 = ToDigits(string_view{ &digits[i - length], length });
					break;
				case 7: // '8' has 7 segments
					D_8 = ToDigits(string_view{ &digits[i - length], length });
					break;
				}

				length = 0;
			}
			else
			{
				charCounts[c - 'a']++;
				length++;
			}
		}
	}

	uint32_t B = 0;
	uint32_t E = 0;
	uint32_t F = 0;
	for (uint32_t i = 0; i < static_cast<uint32_t>(charCounts.size()); i++)
	{
		switch (charCounts[i])
		{
		case 6: // B segment appears 6 times
			B = 1u << i;
			break;
		case 4: // E segment appears 4 times
			E = 1u << i;
			break;
		case 9: // F segment appears 9 times
			F = 1u << i;
			break;
		}
	}

	// A = '7' - '1'
	// B = Known
	// C = '1' - F
	// D = '4' - B - C - F
	// E = Known
	// F = Known
	// G = '8' - A - B - C - D - E - F

	uint32_t A = D_7 & ~D_1;
	uint32_t C = D_1 & ~F;
	uint32_t D = D_4 & ~B & ~C & ~F;
	uint32_t G = D_8 & ~A & ~B & ~C & ~D & ~E & ~F;

	(*mapping)[(A | B | C | E | F | G)] = 0;
	(*mapping)[(C | F)] = 1;
	(*mapping)[(A | C | D | E | G)] = 2;
	(*mapping)[(A | C | D | F | G)] = 3;
	(*mapping)[(B | C | D | F)] = 4;
	(*mapping)[(A | B | D | F | G)] = 5;
	(*mapping)[(A | B | D | E | F | G)] = 6;
	(*mapping)[(A | C | F)] = 7;
	(*mapping)[(A | B | C | D | E | F | G)] = 8;
	(*mapping)[(A | B | C | D | F | G)] = 9;
}

void Puzzle08_A_2021()
{
	array<int32_t, 8> lengths{};
	while (PuzzleInput::NextLine())
	{
		for (int c = PuzzleInput::GetChar(); c != '|'; c = PuzzleInput::GetChar())
			;

		PuzzleInput::DropChar();

		int32_t length = 0;
		for (char c : Parse::ReadUntilSeen('\n'))
		{
			if (c == ' ')
			{
				lengths[length]++;
				length = 0;
			}
			else
			{
				length++;
			}
		}

		lengths[length]++;
	}

	int32_t answer = lengths[2] + lengths[4] + lengths[3] + lengths[7];;

	PuzzleOutput::Submit(2021, 8, 1, answer);
}

void Puzzle08_B_2021()
{
	const size_t signalPatternSize = 59;
	const size_t mappingSize = 128;

	int32_t answer = 0;

	vector<char> signalPatterns(signalPatternSize);
	while (PuzzleInput::NextLine())
	{
		for (size_t i = 0; i < signalPatternSize; i++)
		{
			signalPatterns[i] = static_cast<char>(PuzzleInput::GetChar());
		}
		Parse::DiscardExpected("| ");

		vector<int32_t> mapping(mappingSize);
		Solve(signalPatterns, &mapping);

		int32_t outputNumber = 0;
		uint32_t digits = 0;
		while (true)
		{
			char c = static_cast<char>(PuzzleInput::GetChar());
			if ((c == ' ') || (c == '\n'))
			{
				outputNumber *= 10;
				outputNumber += mapping[digits];
				digits = 0;
			}
			else
			{
				digits |= 1 << (c - 'a');
			}

			if (c == '\n')
				break;
		}

		answer += outputNumber;
	}
	
	PuzzleOutput::Submit(2021, 8, 2, answer);
}

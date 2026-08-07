#include "stdafx.h"

using namespace std;

namespace Puzzle02_2015_Types
{
}

using namespace Puzzle02_2015_Types;

void Puzzle02_A_2015()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		int32_t length = Parse::GetInt32();
		int32_t width = Parse::GetInt32();
		int32_t height = Parse::GetInt32();

		int32_t lw = length * width;
		int32_t wh = width * height;
		int32_t hl = height * length;

		answer += (2 * lw) + (2 * wh) + (2 * hl) + min(min(lw, wh), hl);
	}
	return PuzzleOutput::Submit(2015, 2, 1, answer);
}

void Puzzle02_B_2015()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		int32_t a = Parse::GetInt32(); // Will be the smallest after sorting
		int32_t b = Parse::GetInt32();
		int32_t c = Parse::GetInt32(); // Will be the bigger after sorting

		if (a > c) std::swap(a, c);
		if (a > b) std::swap(a, b);
		if (b > c) std::swap(b, c);

		int32_t around = (2 * a) + (2 * b);
		int32_t bow = a * b * c;

		answer += around + bow;
	}
	return PuzzleOutput::Submit(2015, 2, 2, answer);
}

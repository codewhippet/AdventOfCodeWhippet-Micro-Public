#include "stdafx.h"

using namespace std;

namespace Puzzle21_2018_Types
{
}

using namespace Puzzle21_2018_Types;

static uint32_t Hash(uint32_t r4, uint32_t puzzleConstant)
{
	uint32_t r3 = r4 | 65536;
	r4 = puzzleConstant;

	r4 = ((r4 + (r3 & 0xff)) * 65899);
	r3 >>= 8;
	r4 = ((r4 + (r3 & 0xff)) * 65899);
	r3 >>= 8;
	r4 = ((r4 + (r3 & 0xff)) * 65899) & 0xffffff;

	return r4;
}

static uint32_t FindValueBeforeCycle(uint32_t puzzleConstant)
{
	uint32_t r4 = 0;

	uint32_t prev = 0;
	HashSet<uint32_t> seen(32 * 1024, numeric_limits<uint32_t>::max());
	while (seen.Insert(r4))
	{
		prev = r4;
		r4 = Hash(r4, puzzleConstant);
	}

	return prev;
}

void Puzzle21_A_2018()
{
	for (size_t i = 0; i < 8; i++)
	{
		PuzzleInput::DropLine();
		PuzzleInput::GetChar();
	}

	uint32_t puzzleConstant = Parse::GetUint32();
	uint32_t answer = Hash(0, puzzleConstant);

	PuzzleOutput::Submit(2018, 21, 1, static_cast<int32_t>(answer));
}

void Puzzle21_B_2018()
{
	for (size_t i = 0; i < 8; i++)
	{
		PuzzleInput::DropLine();
		PuzzleInput::GetChar();
	}

	uint32_t puzzleConstant = Parse::GetUint32();
	uint32_t answer = FindValueBeforeCycle(puzzleConstant);

	PuzzleOutput::Submit(2018, 21, 2, static_cast<int32_t>(answer));
}

#include "stdafx.h"

using namespace std;

namespace Puzzle06_2021_Types
{
}

using namespace Puzzle06_2021_Types;

static int64_t LanternFishAfter(size_t days)
{
	vector<int64_t> lanternfish(days + 9);
	while (PuzzleInput::PeekChar() != '\n')
	{
		lanternfish[Parse::GetInt32()]++;
	}

	int64_t* head = &lanternfish[9];
	for (size_t i = 0; i < days; i++)
	{
		int64_t prev = head[-9];
		head[-2] += prev;
		*head++ = prev;
	}

	int64_t total = 0;
	for (int32_t i = -1; i >= -9; i--)
	{
		total += head[i];
	}

	return total;
}

void Puzzle06_A_2021()
{
	int64_t answer = LanternFishAfter(80);

	PuzzleOutput::Submit(2021, 6, 1, answer);
}

void Puzzle06_B_2021()
{
	int64_t answer = LanternFishAfter(256);

	PuzzleOutput::Submit(2021, 6, 2, answer);
}

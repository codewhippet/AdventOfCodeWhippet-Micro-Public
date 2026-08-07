#include "stdafx.h"

using namespace std;

namespace Puzzle03_2016_Types
{
}

using namespace Puzzle03_2016_Types;

void Puzzle03_A_2016()
{
	int32_t answer = 0;
	int32_t t[3];
	while (PuzzleInput::NextLine())
	{
		t[0] = Parse::GetInt32();
		t[1] = Parse::GetInt32();
		t[2] = Parse::GetInt32();
		if ((t[0] + t[1] > t[2]) &&
			(t[1] + t[2] > t[0]) &&
			(t[2] + t[0] > t[1]))
		{
			answer++;
		}
	}

	return PuzzleOutput::Submit(2016, 3, 1, answer);
}

void Puzzle03_B_2016()
{
	int32_t answer = 0;
	int32_t t[3][3];
	while (PuzzleInput::NextLine())
	{
		for (size_t i = 0; i < 3; i++)
		{
			t[0][i] = Parse::GetInt32();
			t[1][i] = Parse::GetInt32();
			t[2][i] = Parse::GetInt32();
		}

		for (size_t i = 0; i < 3; i++)
		{
			if ((t[i][0] + t[i][1] > t[i][2]) &&
				(t[i][1] + t[i][2] > t[i][0]) &&
				(t[i][2] + t[i][0] > t[i][1]))
			{
				answer++;
			}
		}
	}

	return PuzzleOutput::Submit(2016, 3, 2, answer);
}

#include "stdafx.h"

using namespace std;

namespace Puzzle10_2022_Types
{
}

using namespace Puzzle10_2022_Types;

static vector<int32_t> GetCycleValues()
{
	const size_t expectedCycleValues = 256;
	vector<int32_t> cycleValues;
	cycleValues.reserve(expectedCycleValues);

	int32_t xRegister = 1;
	while (PuzzleInput::NextLine())
	{
		if (PuzzleInput::GetChar() == 'n')
		{
			Parse::DiscardExpected("oop");
			cycleValues.push_back(xRegister);
		}
		else
		{
			int32_t value = Parse::GetInt32();

			cycleValues.push_back(xRegister);
			cycleValues.push_back(xRegister);

			xRegister += value;
		}
	}

	return cycleValues;
}

void Puzzle10_A_2022()
{
	vector<int32_t> cycleValues = GetCycleValues();

	int32_t answer =
		cycleValues[20 - 1] * 20 +
		cycleValues[60 - 1] * 60 +
		cycleValues[100 - 1] * 100 +
		cycleValues[140 - 1] * 140 +
		cycleValues[180 - 1] * 180 +
		cycleValues[220 - 1] * 220;

	PuzzleOutput::Submit(2022, 10, 1, answer);
}

void Puzzle10_B_2022()
{
	vector<int32_t> cycleValues = GetCycleValues();

	const size_t expectedNumLines = 6;
	string answer;
	answer.reserve(cycleValues.size() + expectedNumLines + 1);
	for (int32_t i = 0; i < static_cast<int32_t>(cycleValues.size()); i++)
	{
		int32_t vOffset = i % 40;
		if (vOffset == 0)
			answer += '\n';

		int32_t delta = cycleValues[i] - vOffset;
		bool on = (delta == -1) || (delta == 0) || (delta == 1);
		answer += on ? '#' : '.';
	}

	PuzzleOutput::Submit(2022, 10, 2, answer.c_str());
}

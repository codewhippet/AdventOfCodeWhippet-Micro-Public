#include "stdafx.h"

using namespace std;

namespace Puzzle24_2021_Types
{
	struct Stage
	{
		int32_t Divisor;
		int32_t TestOffset;
		int32_t Increment;
	};
}

using namespace Puzzle24_2021_Types;

static vector<Stage> ExtractStages()
{
	const size_t numStages = 14;
	const int32_t linesPerStage = 18;

	vector<Stage> stages(numStages);
	for (size_t stage = 0; stage < numStages; stage++)
	{
		for (int32_t line = 0; line < linesPerStage; line++)
		{
			PuzzleInput::NextLine();
			switch (line)
			{
			case 4:
				stages[stage].Divisor = Parse::GetInt32();
				break;
			case 5:
				stages[stage].TestOffset = Parse::GetInt32();
				break;
			case 15:
				stages[stage].Increment = Parse::GetInt32();
				break;
			default:
				PuzzleInput::DropLine();
				break;
			}
		}
	}

	return stages;
}

static int32_t ExecuteStage(const Stage& s, int32_t input, int32_t z)
{
	int32_t w = input;
	int32_t x = z % 26;
	z = z / s.Divisor;

	bool mismatch = w != (x + s.TestOffset);
	if (mismatch)
	{
		z = (z * 26) + w + s.Increment;
	}

	return z;
}

static pair<int32_t, int32_t> LargestPair(const Stage& stageA, const Stage& stageB)
{
	for (int32_t i = 9; i > 0; i--)
	{
		for (int32_t j = 9; j > 0; j--)
		{
			int32_t z0 = 0;
			int32_t z1 = ExecuteStage(stageA, i, z0);
			int32_t z2 = ExecuteStage(stageB, j, z1);

			if (z2 == 0)
			{
				return { i, j };
			}
		}
	}

	return { -1, -1 };
}

static pair<int32_t, int32_t> SmallestPair(const Stage& stageA, const Stage& stageB)
{
	for (int32_t i = 1; i < 10; i++)
	{
		for (int32_t j = 1; j < 10; j++)
		{
			int32_t z0 = 0;
			int32_t z1 = ExecuteStage(stageA, i, z0);
			int32_t z2 = ExecuteStage(stageB, j, z1);

			if (z2 == 0)
			{
				return { i, j };
			}
		}
	}

	return { -1, -1 };
}

void Puzzle24_A_2021()
{
	vector<Stage> stages = ExtractStages();

	const size_t numStagePairs = stages.size() / 2;
	vector<size_t> stack;
	stack.reserve(numStagePairs);

	vector<int32_t> digits(stages.size());
	for (size_t i = 0; i < stages.size(); i++)
	{
		if (stages[i].Divisor == 1)
		{
			stack.push_back(i);
		}
		else
		{
			size_t j = stack.back();
			stack.pop_back();

			pair<int32_t, int32_t> smallestDigits = LargestPair(stages[j], stages[i]);
			digits[j] = smallestDigits.first;
			digits[i] = smallestDigits.second;
		}
	}
	assert(stack.empty());

	int64_t answer = 0;
	for (int32_t digit : digits)
	{
		answer = answer * 10 + digit;
	}

	PuzzleOutput::Submit(2021, 24, 1, answer);
}

void Puzzle24_B_2021()
{
	vector<Stage> stages = ExtractStages();

	const size_t numStagePairs = stages.size() / 2;
	vector<size_t> stack;
	stack.reserve(numStagePairs);

	vector<int32_t> digits(stages.size());
	for (size_t i = 0; i < stages.size(); i++)
	{
		if (stages[i].Divisor == 1)
		{
			stack.push_back(i);
		}
		else
		{
			size_t j = stack.back();
			stack.pop_back();

			pair<int32_t, int32_t> smallestDigits = SmallestPair(stages[j], stages[i]);
			digits[j] = smallestDigits.first;
			digits[i] = smallestDigits.second;
		}
	}
	assert(stack.empty());

	int64_t answer = 0;
	for (int32_t digit : digits)
	{
		answer = answer * 10 + digit;
	}

	PuzzleOutput::Submit(2021, 24, 2, answer);
}

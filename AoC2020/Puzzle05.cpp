#include "stdafx.h"

using namespace std;

namespace Puzzle05_2020_Types
{
}

using namespace Puzzle05_2020_Types;

void Puzzle05_A_2020()
{
	MaxValue<int32_t> answer;

	int32_t value = 0;
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case '\n':
			answer.Update(value);
			value = 0;
			break;

		case 'F':
		case 'L':
			value = (value << 1);
			break;

		case 'B':
		case 'R':
			value = (value << 1) | 1;
			break;
		}
	}

	PuzzleOutput::Submit(2020, 5, 1, answer.Get());
}

void Puzzle05_B_2020()
{
	vector<int32_t> passes;
	passes.reserve(1024);

	int32_t value = 0;
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case '\n':
			passes.push_back(value);
			value = 0;
			break;

		case 'F':
		case 'L':
			value = (value << 1);
			break;

		case 'B':
		case 'R':
			value = (value << 1) | 1;
			break;
		}
	}

	sort(passes.begin(), passes.end());

	int32_t answer = 0;
	for (size_t i = 0; i + 1 < passes.size(); i++)
	{
		if (passes[i + 1] != (passes[i] + 1))
		{
			answer = passes[i] + 1;
			break;
		}
	}

	PuzzleOutput::Submit(2020, 5, 2, answer);
}

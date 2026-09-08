#include "stdafx.h"

using namespace std;

namespace Puzzle06_2020_Types
{
}

using namespace Puzzle06_2020_Types;

void Puzzle06_A_2020()
{
	int32_t answer = 0;

	array<int32_t, 26> seen = {};
	while (PuzzleInput::PeekChar() != EOF)
	{
		if (PuzzleInput::PeekChar() == '\n')
		{
			answer += accumulate(seen.begin(), seen.end(), 0);
			ranges::fill(seen, 0);
		}

		for (char c : Parse::ReadUntilSeen('\n'))
		{
			seen[c - 'a'] = 1;
		}
	}

	answer += accumulate(seen.begin(), seen.end(), 0);

	PuzzleOutput::Submit(2020, 6, 1, answer);
}

void Puzzle06_B_2020()
{
	int32_t answer = 0;

	int32_t numPeople = 0;
	array<int32_t, 26> seen = {};
	while (PuzzleInput::PeekChar() != EOF)
	{
		if (PuzzleInput::PeekChar() == '\n')
		{
			int32_t allAnswered = static_cast<int32_t>(count(seen.begin(), seen.end(), numPeople));
			answer += allAnswered;

			ranges::fill(seen, 0);
			numPeople = 0;

			PuzzleInput::DropChar();
			continue;
		}

		for (char c : Parse::ReadUntilSeen('\n'))
		{
			seen[c - 'a']++;
		}

		numPeople++;
	}

	int32_t allAnswered = static_cast<int32_t>(count(seen.begin(), seen.end(), numPeople));
	answer += allAnswered;

	PuzzleOutput::Submit(2020, 6, 2, answer);
}

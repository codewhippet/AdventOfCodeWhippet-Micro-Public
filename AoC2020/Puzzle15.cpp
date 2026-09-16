#include "stdafx.h"

using namespace std;

namespace Puzzle15_2020_Types
{
}

using namespace Puzzle15_2020_Types;

static int32_t NthNumberSpoken(int32_t n)
{
	vector<int32_t> spokenHistory;
	spokenHistory.resize(n, -1);

	int32_t turn = 0;
	int32_t lastSpoken = 0;

	while (PuzzleInput::PeekChar() != '\n')
	{
		int32_t value = Parse::GetInt32();
		spokenHistory[value] = turn++;
		lastSpoken = value;
	}

	spokenHistory[lastSpoken] = -1; // Pretend we haven't recorded the last spoken word yet

	for (/***/; turn < n; turn++)
	{
		int32_t valueToSpeak = -1;

		int32_t lastSpokenWasSpokenBeforeAt = spokenHistory[lastSpoken];
		if (lastSpokenWasSpokenBeforeAt == -1)
		{
			valueToSpeak = 0;
		}
		else
		{
			valueToSpeak = turn - lastSpokenWasSpokenBeforeAt - 1;
		}

		spokenHistory[lastSpoken] = turn - 1;
		lastSpoken = valueToSpeak;
	}

	return lastSpoken;
}

void Puzzle15_A_2020()
{
	int32_t answer = NthNumberSpoken(2020);
	PuzzleOutput::Submit(2020, 15, 1, answer);
}

void Puzzle15_B_2020()
{
#if PICO_ON_DEVICE

	PuzzleOutput::Unsupported(2020, 15, 2);

#else

	int32_t answer = NthNumberSpoken(30000000);
	PuzzleOutput::Submit(2020, 15, 2, answer);

#endif
}

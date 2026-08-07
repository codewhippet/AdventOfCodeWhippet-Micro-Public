#include "stdafx.h"

using namespace std;

namespace Puzzle04_2017_Types
{
}

using namespace Puzzle04_2017_Types;

static bool IsValidPartOne(const char* passphrase)
{
	const char* wordStart = passphrase;

	HashSet<string_view> words(64, {});
	while (true)
	{
		const char* wordEnd = wordStart + 1;
		while (isalpha(*wordEnd))
			wordEnd++;

		string_view w(wordStart, wordEnd);
		if (words.Insert(w) == false)
		{
			return false;
		}

		if (*wordEnd == '\0')
			break;

		wordStart = wordEnd + 1;
	}

	return true;
}

static bool IsValidPartTwo(char* passphrase)
{
	char* wordStart = passphrase;

	HashSet<string_view> words(64, {});
	while (true)
	{
		char* wordEnd = wordStart + 1;
		while (isalpha(*wordEnd))
			wordEnd++;

		span w(wordStart, wordEnd);
		sort(w.begin(), w.end());

		if (words.Insert(string_view{ w.begin(), w.end() }) == false)
		{
			return false;
		}

		if (*wordEnd == '\0')
			break;

		wordStart = wordEnd + 1;
	}

	return true;
}

void Puzzle04_A_2017()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		char line[128];
		Parse::ReadNonEmptyLine(line);

		if (IsValidPartOne(line))
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2017, 4, 1, answer);
}

void Puzzle04_B_2017()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		char line[128];
		Parse::ReadNonEmptyLine(line);

		if (IsValidPartTwo(line))
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2017, 4, 2, answer);
}

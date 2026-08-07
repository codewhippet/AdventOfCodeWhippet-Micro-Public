#include "stdafx.h"

using namespace std;

namespace Puzzle05_2015_Types
{
}

using namespace Puzzle05_2015_Types;

void Puzzle05_A_2015()
{
	const regex vowels(R"(a|e|i|o|u)");
	const regex doubleLetter(R"(([a-z])\1)");
	const regex containsForbidden(R"(ab|cd|pq|xy)");

	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		char line[32];
		int32_t lineSize = Parse::ReadLine(line, size(line));
		assert(lineSize > 0);

		const char* lineBegin = &line[0];
		const char* lineEnd = lineBegin + lineSize;

		size_t numVowels = distance(cregex_iterator{ lineBegin, lineEnd, vowels }, cregex_iterator{});
		bool hasDoubleLetter = regex_search(lineBegin, lineEnd, doubleLetter);
		bool hasForbidden = regex_search(lineBegin, lineEnd , containsForbidden);

		if ((numVowels >= 3) && hasDoubleLetter && !hasForbidden)
		{
			answer++;
		}
	}

	return PuzzleOutput::Submit(2015, 5, 1, answer);
}

void Puzzle05_B_2015()
{
	const regex doubleDouble(R"(([a-z]{2}).*\1)");
	const regex repeatedLetter(R"(([a-z]).\1)");

	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		char line[32];
		int32_t lineSize = Parse::ReadLine(line, size(line));
		assert(lineSize > 0);

		const char* lineBegin = &line[0];
		const char* lineEnd = lineBegin + lineSize;

		bool hasDoubleLetter = regex_search(lineBegin, lineEnd, doubleDouble);
		bool hasRepeated = regex_search(lineBegin, lineEnd, repeatedLetter);

		if (hasDoubleLetter && hasRepeated)
		{
			answer++;
		}
	}

	return PuzzleOutput::Submit(2015, 5, 2, answer);
}

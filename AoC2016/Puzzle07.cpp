#include "stdafx.h"

using namespace std;

namespace Puzzle07_2016_Types
{
}

using namespace Puzzle07_2016_Types;

static vector<string> SplitIntoSequences(const string_view& s, const string_view& separator)
{
	vector<string> sequences(2);
	sequences[0].reserve(s.size() + 64);
	sequences[1].reserve(s.size() + 64);

	size_t sequenceIndex = 0;
	for (char c : s)
	{
		switch (c)
		{
		case '[':
		case ']':
			sequences[sequenceIndex] += separator;
			sequenceIndex = 1 - sequenceIndex;
			break;
		default:
			sequences[sequenceIndex] += c;
			break;
		}
	}
	return sequences;
}

static bool ContainsAbba(const string& s)
{
	for (size_t i = 0; i + 3 < s.size(); i++)
	{
		if ((s[i + 0] == s[i + 3]) &&
			(s[i + 1] == s[i + 2]) &&
			(s[i + 0] != s[i + 1]))
		{
			return true;
		}
	}
	return false;
}

static size_t FindNextAba(size_t startFrom, const string& s)
{
	for (size_t i = startFrom; i + 2 < s.size(); i++)
	{
		if ((s[i + 0] == s[i + 2]) &&
			(s[i + 0] != s[i + 1]))
		{
			return i;
		}
	}
	return numeric_limits<size_t>::max();
}

void Puzzle07_A_2016()
{
	int32_t answer = 0;

	char line[256];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line, sizeof(line));

		vector<string> sequences = SplitIntoSequences(line, ".");
		if (ContainsAbba(sequences[0]) && !ContainsAbba(sequences[1]))
		{
			answer++;
		}
	}

	return PuzzleOutput::Submit(2016, 7, 1, answer);
}

void Puzzle07_B_2016()
{
	int32_t answer = 0;

	char line[256];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line, sizeof(line));

		vector<string> sequences = SplitIntoSequences(line, "..");
		for (size_t abaIndex = FindNextAba(0, sequences[0]); abaIndex != numeric_limits<size_t>::max(); abaIndex = FindNextAba(abaIndex + 1, sequences[0]))
		{
			const char bab[] = { sequences[0][abaIndex + 1], sequences[0][abaIndex + 0], sequences[0][abaIndex + 1], '\0' };
			if (sequences[1].find(bab) != string::npos)
			{
				answer++;
				break;
			}
		}
	}

	return PuzzleOutput::Submit(2016, 7, 2, answer);
}

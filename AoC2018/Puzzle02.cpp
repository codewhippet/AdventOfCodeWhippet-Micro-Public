#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle02_2018_Types
{
}

using namespace Puzzle02_2018_Types;

static string RemoveDifferences(const string& a, const string& b)
{
	assert(a.size() == b.size());

	string same;
	same.reserve(a.size() - 1);
	for (size_t i = 0; i < a.size(); i++)
	{
		if (a[i] == b[i])
		{
			same += a[i];
		}
	}
	return same;
}

static int32_t CountDifferences(const string& a, const string& b)
{
	int32_t diffs = 0;

	assert(a.size() == b.size());
	for (size_t i = 0; i < a.size(); i++)
	{
		if (a[i] != b[i])
		{
			diffs++;
		}
	}

	return diffs;
}

void Puzzle02_A_2018()
{
	int32_t twoCounts = 0;
	int32_t threeCounts = 0;
	while (PuzzleInput::NextLine())
	{
		array<int32_t, 26> charCounts = {};
		for (int c = PuzzleInput::GetChar(); c != '\n'; c = PuzzleInput::GetChar())
		{
			charCounts[c - 'a']++;
		}
		twoCounts += (ranges::find(charCounts, 2) != charCounts.end());
		threeCounts += (ranges::find(charCounts, 3) != charCounts.end());
	}

	int32_t answer = twoCounts * threeCounts;
	PuzzleOutput::Submit(2018, 2, 1, answer);
}

void Puzzle02_B_2018()
{
	vector<string> lines;
	lines.reserve(250);

	char line[32];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);
		lines.push_back(line);
	}

	string answer;

	for (const auto& p : AllUnorderedPairs(lines.size()))
	{
		if (CountDifferences(lines[p.first], lines[p.second]) == 1)
		{
			answer = RemoveDifferences(lines[p.first], lines[p.second]);
			break;
		}
	}

	PuzzleOutput::Submit(2018, 2, 2, answer.c_str());
}

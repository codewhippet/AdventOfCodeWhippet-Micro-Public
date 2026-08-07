#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle12_2018_Types
{
	struct Puzzle
	{
		string InitialState;
		set<string> PositiveStates;
	};
}

using namespace Puzzle12_2018_Types;

static string SingleMatch(const string& source, const regex& pattern)
{
	smatch m;
	regex_match(source, m, pattern);
	return m[1].str();
}

static Puzzle ReadPuzzle()
{
	Puzzle p;

	char line[128];
	Parse::ReadNonEmptyLine(line);

	p.InitialState = SingleMatch(line, regex{ R"(initial state: ([#\.]+))" });

	regex rule{ R"(([#\.]{5}) => #)" };
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);
		p.PositiveStates.insert(SingleMatch(line, rule));
	}

	return p;
}

void Puzzle12_A_2018()
{
	const size_t generations = 20;

	Puzzle p = ReadPuzzle();

	string pots = string(generations, '.') + p.InitialState + string(generations, '.');

	for (size_t gen = 0; gen < generations; gen++)
	{
		string newPots = "..";
		for (size_t i = 2; i < pots.size(); i++)
		{
			char n = p.PositiveStates.contains(pots.substr(i - 2, 5)) ? '#' : '.';
			newPots += n;
		}
		pots.swap(newPots);
	}

	size_t answer = 0;
	for (size_t i = 0; i < pots.size(); i++)
	{
		if (pots[i] == '#')
		{
			size_t potIndex = i - generations;
			answer += potIndex;
		}
	}

	PuzzleOutput::Submit(2018, 12, 1, answer);
}

void Puzzle12_B_2018()
{
	const int64_t totalGenerations = 50000000000;
	const size_t startingGenerations = 200;
	const int64_t remainingGenerations = totalGenerations - startingGenerations;

	Puzzle p = ReadPuzzle();

	string pots = string(startingGenerations, '.') + p.InitialState + string(startingGenerations, '.');

	for (size_t gen = 0; gen < startingGenerations; gen++)
	{
		string newPots = "..";
		for (size_t i = 2; i < pots.size(); i++)
		{
			char n = p.PositiveStates.contains(pots.substr(i - 2, 5)) ? '#' : '.';
			newPots += n;
		}
		pots.swap(newPots);
	}

	size_t startingScore = 0;
	for (size_t i = 0; i < pots.size(); i++)
	{
		if (pots[i] == '#')
		{
			size_t potIndex = i - startingGenerations;
			startingScore += potIndex;
		}
	}

	size_t stableCount = count(pots.begin(), pots.end(), '#');

	int64_t answer = startingScore + stableCount * remainingGenerations;

	PuzzleOutput::Submit(2018, 12, 2, answer);
}

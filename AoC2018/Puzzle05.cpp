#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle05_2018_Types
{
}

using namespace Puzzle05_2018_Types;

static bool IsSameLetterButOppositeCase(char a, char b)
{
	return (a ^ b) == 0x20;
}

static bool IsSameLetterIgnoreCase(char a, char b)
{
	return ((a | 0x20) ^ (b | 0x20)) == 0;
}

static string FullyReduceInput()
{
	string polymer;
	polymer.reserve(64 * 1024);

	for (int c = PuzzleInput::GetChar(); (c != '\n') && (c != EOF); c = PuzzleInput::GetChar())
	{
		if (!polymer.empty() && IsSameLetterButOppositeCase(static_cast<char>(c), polymer.back()))
		{
			polymer.pop_back();
		}
		else
		{
			polymer.push_back(static_cast<char>(c));
		}
	}

	return polymer;
}

static size_t GetReducedSize(const string& oldPolymer, char unitToIgnore)
{
	string newPolymer;
	newPolymer.reserve(oldPolymer.size());

	for (char c : oldPolymer)
	{
		if (IsSameLetterIgnoreCase(c, unitToIgnore))
			continue;

		if (!newPolymer.empty() && IsSameLetterButOppositeCase(c, newPolymer.back()))
		{
			newPolymer.pop_back();
		}
		else
		{
			newPolymer.push_back(c);
		}
	}

	return newPolymer.size();
}

void Puzzle05_A_2018()
{
	string reducedPolymer = FullyReduceInput();

	PuzzleOutput::Submit(2018, 5, 1, reducedPolymer.size());
}

void Puzzle05_B_2018()
{
	string reducedPolymer = FullyReduceInput();

	size_t answer = numeric_limits<size_t>::max();
	for (char c = 'A'; c <= 'Z'; c++)
	{
		answer = min(answer, GetReducedSize(reducedPolymer, c));
	}

	PuzzleOutput::Submit(2018, 5, 2, answer);
}

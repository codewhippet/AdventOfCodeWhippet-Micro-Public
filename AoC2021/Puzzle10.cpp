#include "stdafx.h"

using namespace std;

namespace Puzzle10_2021_Types
{
}

using namespace Puzzle10_2021_Types;

static bool IsOpen(char c)
{
	return
		c == '(' ||
		c == '[' ||
		c == '{' ||
		c == '<';
}

static char FirstCorrupt(vector<char>* openSet, const vector<char>& openingFor)
{
	while (PuzzleInput::PeekChar() != '\n')
	{
		char c = static_cast<char>(PuzzleInput::GetChar());

		if (IsOpen(c))
		{
			openSet->push_back(c);
			continue;
		}

		if (openSet->empty())
		{
			return c;
		}

		if (openSet->back() != openingFor[c])
		{
			return c;
		}

		openSet->pop_back();
	}

	return '\0';
}

static void GetClosingChars(vector<char>* openSet, const vector<char>& openingFor)
{
	while (PuzzleInput::PeekChar() != '\n')
	{
		char c = static_cast<char>(PuzzleInput::GetChar());

		if (IsOpen(c))
		{
			openSet->push_back(c);
			continue;
		}

		if (openSet->empty())
		{
			return;
		}

		if (openSet->back() != openingFor[c])
		{
			openSet->clear();
			return;
		}

		openSet->pop_back();
	}
}

void Puzzle10_A_2021()
{
	const size_t charSetSize = 128;
	const size_t maxOpenSetSize = 32;

	vector<char> openingFor(charSetSize);
	openingFor[')'] = '(';
	openingFor[']'] = '[';
	openingFor['}'] = '{';
	openingFor['>'] = '<';

	vector<int32_t> pointsForCorruption(charSetSize);
	pointsForCorruption[')'] = 3;
	pointsForCorruption[']'] = 57;
	pointsForCorruption['}'] = 1197;
	pointsForCorruption['>'] = 25137;

	vector<char> openSet;
	openSet.reserve(maxOpenSetSize);

	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		openSet.clear();
		char c = FirstCorrupt(&openSet, openingFor);
		if (c)
		{
			answer += pointsForCorruption[c];
		}
		PuzzleInput::DropLine();
	}
	
	PuzzleOutput::Submit(2021, 10, 1, answer);
}

void Puzzle10_B_2021()
{
	const size_t charSetSize = 128;
	const size_t maxOpenSetSize = 32;
	const size_t maxClosingScores = 64;

	vector<char> openingFor(charSetSize);
	openingFor[')'] = '(';
	openingFor[']'] = '[';
	openingFor['}'] = '{';
	openingFor['>'] = '<';

	vector<char> closingFor(charSetSize);
	closingFor['('] = ')';
	closingFor['['] = ']';
	closingFor['{'] = '}';
	closingFor['<'] = '>';

	vector<int32_t> pointsForClosing(charSetSize);
	pointsForClosing[')'] = 1;
	pointsForClosing[']'] = 2;
	pointsForClosing['}'] = 3;
	pointsForClosing['>'] = 4;

	vector<char> openSet;
	openSet.reserve(maxOpenSetSize);

	vector<int64_t> closingScores;
	closingScores.reserve(maxClosingScores);

	while (PuzzleInput::NextLine())
	{
		openSet.clear();
		GetClosingChars(&openSet, openingFor);
		if (openSet.size() > 0)
		{
			int64_t closingScore = 0;
			while (openSet.size() > 0)
			{
				closingScore = closingScore * 5 + pointsForClosing[closingFor[openSet.back()]];
				openSet.pop_back();
			}

			closingScores.push_back(closingScore);
		}

		PuzzleInput::DropLine();
	}

	size_t middleElement = (closingScores.size() / 2);
	nth_element(closingScores.begin(),
		closingScores.begin() + middleElement,
		closingScores.end());

	int32_t answer = static_cast<int32_t>(closingScores[middleElement]);

	PuzzleOutput::Submit(2021, 10, 2, answer);
}

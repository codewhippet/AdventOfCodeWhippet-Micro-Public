#include "stdafx.h"

using namespace std;

namespace Puzzle06_2016_Types
{
}

using namespace Puzzle06_2016_Types;

static void InitialiseCharacterCounts(array<pair<int32_t, char>, 26>* characterCounts)
{
	for (size_t i = 0; i < characterCounts->size(); i++)
	{
		(*characterCounts)[i] = { 0, static_cast<char>('a' + i) };
	}
}

static string Decode(int32_t weighting)
{
	array<array<pair<int32_t, char>, 26>, 8> columnCounts;
	for (auto& counts : columnCounts)
	{
		InitialiseCharacterCounts(&counts);
	}

	char line[9];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line, sizeof(line));
		for (size_t column = 0; column < sizeof(line) - 1; column++)
		{
			columnCounts[column][(line[column] - 'a')].first += weighting;
		}
	}

	string decoded = "........";
	for (size_t i = 0; i < 8; i++)
	{
		ranges::sort(columnCounts[i]);
		decoded[i] = columnCounts[i].front().second;
	}

	return decoded;
}

void Puzzle06_A_2016()
{
	string answer = Decode(-1);
	return PuzzleOutput::Submit(2016, 6, 1, answer.c_str());
}

void Puzzle06_B_2016()
{
	string answer = Decode(1);
	return PuzzleOutput::Submit(2016, 6, 2, answer.c_str());
}

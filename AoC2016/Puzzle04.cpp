#include "stdafx.h"

using namespace std;

namespace Puzzle04_2016_Types
{
}

using namespace Puzzle04_2016_Types;

static void InitialiseCharacterCounts(array<pair<int32_t, char>, 26>* characterCounts)
{
	for (size_t i = 0; i < characterCounts->size(); i++)
	{
		(*characterCounts)[i] = { 0, static_cast<char>('a' + i) };
	}
}

static bool DoesChecksumMatch(const array<pair<int32_t, char>, 26>& characterCounts, const csub_match& match)
{
	for (size_t i = 0; i < 5; i++)
	{
		if (characterCounts[i].second != *(match.first + i))
		{
			return false;
		}
	}
	return true;
}

void Puzzle04_A_2016()
{
	int32_t answer = 0;

	const regex linePattern{ R"(([[:alpha:]\-]+)(\d+)\[(\w+)\])" };

	array<pair<int32_t, char>, 26> characterCounts;

	char line[96];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line, sizeof(line));
		InitialiseCharacterCounts(&characterCounts);

		cmatch match;
		bool matched = regex_match(line, match, linePattern);
		assert(matched);
		(void)matched;

		for_each(match[1].first, match[1].second,
			[&](char c)
			{
				if (c != '-')
				{
					characterCounts[(c - 'a')].first--;
				}
			});

		ranges::sort(characterCounts);
		if (DoesChecksumMatch(characterCounts, match[3]))
		{
			answer += atoi(match[2].first);
		}
	}

	return PuzzleOutput::Submit(2016, 4, 1, answer);
}

void Puzzle04_B_2016()
{
	int32_t answer = 0;

	const regex linePattern{ R"(([[:alpha:]\-]+)(\d+).*)" };

	array<pair<int32_t, char>, 26> characterCounts;

	char line[96];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line, sizeof(line));
		int32_t sectorId = abs(Parse::GetInt32(line)); // abs in case there's a '-' in front of the number

		for (char* c = &line[0]; *c; c++)
		{
			int32_t code = *c - 'a';
			code = (code + sectorId) % 26;
			*c = 'a' + (char)code;
		}

		if (strstr(line, "northpole"))
		{
			answer = sectorId;
			break;
		}
	}

	return PuzzleOutput::Submit(2016, 4, 2, answer);
}

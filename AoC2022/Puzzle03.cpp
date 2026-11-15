#include "stdafx.h"

using namespace std;

namespace Puzzle03_2022_Types
{
}

using namespace Puzzle03_2022_Types;

void Puzzle03_A_2022()
{
	const size_t alphabetSize = 128;
	vector<int32_t> priorities(alphabetSize);
	for (int32_t i = 0; i < 26; i++)
	{
		priorities['a' + i] = 1 + i;
		priorities['A' + i] = 27 + i;
	}

	int32_t answer = 0;

	array<char, 64> line;
	while (PuzzleInput::NextLine())
	{
		int32_t len = Parse::ReadNonEmptyLine(line.data(), line.size());

		int32_t halfLen = len / 2;
		array<char, 128> seen = {};
		for (int32_t i = 0; i < halfLen; i++)
		{
			seen[line[i]]++;
		}

		for (int32_t i = halfLen; i < len; i++)
		{
			if (seen[line[i]])
			{
				answer += priorities[line[i]];
				break;
			}
		}
	}
	
	PuzzleOutput::Submit(2022, 3, 1, answer);
}

void Puzzle03_B_2022()
{
	const uint32_t groupSize = 3;

	const size_t alphabetSize = 128;
	vector<int32_t> priorities(alphabetSize);
	for (int32_t i = 0; i < 26; i++)
	{
		priorities['a' + i] = 1 + i;
		priorities['A' + i] = 27 + i;
	}

	int32_t answer = 0;

	array<char, 64> line;
	while (PuzzleInput::NextLine())
	{
		vector<uint32_t> seen(128);

		uint32_t allElves = 0;
		for (uint32_t elf = 0; elf < groupSize - 1; elf++)
		{
			const uint32_t elfMask = 1 << elf;
			allElves |= elfMask;

			int32_t len = Parse::ReadNonEmptyLine(line.data(), line.size());
			for (int32_t i = 0; i < len; i++)
			{
				seen[line[i]] |= elfMask;
			}
		}

		int32_t len = Parse::ReadNonEmptyLine(line.data(), line.size());
		for (int32_t i = 0; i < len; i++)
		{
			if (seen[line[i]] == allElves)
			{
				answer += priorities[line[i]];
				break;
			}
		}
	}

	PuzzleOutput::Submit(2022, 3, 2, answer);
}

#include "stdafx.h"

using namespace std;

namespace Puzzle08_2015_Types
{
}

using namespace Puzzle08_2015_Types;

static int32_t SizeInBytes(const char* s)
{
	int32_t size = 0;

	assert(s[0] == '"');
	s++;

	while (*s != '"')
	{
		if (s[0] == '\\')
		{
			switch (s[1])
			{
			case '\\':
			case '"':
				s += 2;
				break;

			case 'x':
				s += 4;
				break;
			}
		}
		else
		{
			s++;
		}

		size++;
	}

	return size;
}

static int32_t EncodedSize(const string &s)
{
	int32_t size = 2;

	for (char c : s)
	{
		switch (c)
		{
		case '"':
		case '\\':
			size += 2;
			break;

		default:
			size += 1;
			break;
		}
	}

	return size;
}

void Puzzle08_A_2015()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		char line[64];
		int32_t lineLength = Parse::ReadLine(line, sizeof(line));
		assert(lineLength > 0);

		int32_t sizeInBytes = SizeInBytes(line);
		answer += lineLength - sizeInBytes;
	}

	return PuzzleOutput::Submit(2015, 8, 1, answer);
}

void Puzzle08_B_2015()
{
	int32_t answer = 0;
	while (PuzzleInput::NextLine())
	{
		char line[64];
		int32_t lineLength = Parse::ReadLine(line, sizeof(line));
		assert(lineLength > 0);

		int32_t encodedSize = EncodedSize(line);
		answer += encodedSize - lineLength;
	}

	return PuzzleOutput::Submit(2015, 8, 2, answer);
}

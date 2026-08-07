#include "stdafx.h"

using namespace std;

namespace Puzzle01_2017_Types
{
}

using namespace Puzzle01_2017_Types;

static vector<char> ReadLargeLine()
{
	vector<char> captchaLine(4 * 1024);

	size_t numDigits = 0;
	for (/***/; numDigits < captchaLine.size(); numDigits++)
	{
		int c = PuzzleInput::GetChar();
		if (!isdigit(c))
			break;

		captchaLine[numDigits] = static_cast<char>(c);
	}

	assert(numDigits != captchaLine.size());
	captchaLine.resize(numDigits);
	return captchaLine;
}

static size_t AddReduce(size_t index, size_t increment, size_t size)
{
	index += increment;
	assert(index < (size * 2));
	return (index < size ? index : index - size);
}

void Puzzle01_A_2017()
{
	vector<char> captchaLine = ReadLargeLine();

	int32_t answer = 0;
	for (size_t i = 0; i < captchaLine.size(); i++)
	{
		size_t j = AddReduce(i, 1, captchaLine.size());
		if (captchaLine[i] == captchaLine[j])
		{
			answer += captchaLine[i] - '0';
		}
	}

	PuzzleOutput::Submit(2017, 1, 1, answer);
}


void Puzzle01_B_2017()
{
	vector<char> captchaLine = ReadLargeLine();

	const size_t halfLineLength = (captchaLine.size() / 2);

	int32_t answer = 0;
	for (size_t i = 0; i < captchaLine.size(); i++)
	{
		size_t j = AddReduce(i, halfLineLength, captchaLine.size());
		if (captchaLine[i] == captchaLine[j])
		{
			answer += captchaLine[i] - '0';
		}
	}

	PuzzleOutput::Submit(2017, 1, 2, answer);
}

#include "stdafx.h"

using namespace std;

namespace Puzzle09_2016_Types
{
	struct Segment
	{
		size_t Size;
		size_t Repeat;

		size_t CurrentIndex;
		int64_t DecompressedSize;
	};
}

using namespace Puzzle09_2016_Types;

static size_t GetInt(size_t* charsConsumed)
{
	while (!isdigit(PuzzleInput::PeekChar()))
	{
		assert(PuzzleInput::PeekChar() != EOF);
		PuzzleInput::DropChar();
		(*charsConsumed)++;
	}

	size_t result = 0;
	while (isdigit(PuzzleInput::PeekChar()))
	{
		result = (result * 10) + (size_t)(PuzzleInput::GetChar() - '0');
		(*charsConsumed)++;
	}
	return result;
}

static int64_t DecompressedLength()
{
	vector<Segment> evalStack;
	evalStack.reserve(16);
	evalStack.push_back({ numeric_limits<size_t>::max(), 1 });

	size_t maxStack = 0;
	while (true)
	{
		maxStack = max(maxStack, evalStack.size());

		if (evalStack.back().CurrentIndex == evalStack.back().Size)
		{
			size_t segmentConsumed = evalStack.back().Size;
			int64_t segmentDecompressedSize = evalStack.back().DecompressedSize * evalStack.back().Repeat;

			evalStack.pop_back();
			evalStack.back().CurrentIndex += segmentConsumed;
			evalStack.back().DecompressedSize += segmentDecompressedSize;
		}
		else
		{
			int c = PuzzleInput::GetChar();
			if ((c == EOF) || isspace(c))
				break;

			evalStack.back().CurrentIndex++;

			if (c != '(')
			{
				evalStack.back().DecompressedSize++;
			}
			else
			{
				size_t markerSize = 0;
				size_t count = GetInt(&markerSize);
				size_t repeat = GetInt(&markerSize);
				assert(PuzzleInput::PeekChar() == ')');
				PuzzleInput::DropChar();
				markerSize++;

				evalStack.back().CurrentIndex += markerSize;

				evalStack.push_back({ count, repeat });
			}
		}
	}

	assert(evalStack.size() == 1);
	return evalStack.back().DecompressedSize;
}

void Puzzle09_A_2016()
{
	int32_t answer = 0;

	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		if (isspace(c))
			continue;

		if (c != '(')
		{
			answer++;
		}
		else
		{
			int32_t count = Parse::GetInt32();
			int32_t repeat = Parse::GetInt32();
			assert(PuzzleInput::PeekChar() == ')');
			PuzzleInput::DropChar();

			answer += count * repeat;

			for (int32_t i = 0; i < count; i++)
			{
				PuzzleInput::DropChar();
			}
		}
	}

	return PuzzleOutput::Submit(2016, 9, 1, answer);
}

void Puzzle09_B_2016()
{
	int64_t answer = DecompressedLength();
	return PuzzleOutput::Submit(2016, 9, 2, answer);
}

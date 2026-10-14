#include "stdafx.h"

using namespace std;

namespace Puzzle13_2021_Types
{
}

using namespace Puzzle13_2021_Types;

static void ReadDots(HashSet<Vec2Int>* dots)
{
	while (PuzzleInput::PeekChar() != '\n')
	{
		dots->Insert({ Parse::GetInt32(), Parse::GetInt32() });
		PuzzleInput::DropChar();
	}

	PuzzleInput::NextLine();
}

static void FoldDots(const HashSet<Vec2Int>& src, HashSet<Vec2Int>* dest, char axis, int32_t line)
{
	dest->Reset();

	if (axis == 'x')
	{
		for (const Vec2Int& original : src)
		{
			Vec2Int folded = original;
			if (original.X > line)
			{
				int32_t distanceFromFold = original.X - line;
				folded.X = line - distanceFromFold;
			}
			dest->Insert(folded);
		}
	}
	else
	{
		for (const Vec2Int& original : src)
		{
			Vec2Int folded = original;
			if (original.Y > line)
			{
				int32_t distanceFromFold = original.Y - line;
				folded.Y = line - distanceFromFold;
			}
			dest->Insert(folded);
		}
	}
}

static string PrintPuzzle(const HashSet<Vec2Int>& dots)
{
	const size_t maxPuzzleOutput = 6 * 64;

	MaxValue<int32_t> width, height;
	for (const Vec2Int& dot : dots)
	{
		width.Update(dot.X);
		height.Update(dot.Y);
	}

	string ret;
	ret.reserve(maxPuzzleOutput);

	for (int32_t y = 0; y <= height; y++)
	{
		ret += '\n';
		for (int32_t x = 0; x <= width; x++)
		{
			ret += dots.Contains({ x, y }) ? '#' : ' ';
		}
	}

	return ret;
}

void Puzzle13_A_2021()
{
	const uint32_t dotHashSize = 2 * 1024;

	array<HashSet<Vec2Int>, 2> dotBuffers
	{
		HashSet<Vec2Int>(dotHashSize, Vec2Int::Min()),
		HashSet<Vec2Int>(dotHashSize, Vec2Int::Min()),
	};

	ReadDots(&dotBuffers[0]);

	Parse::DiscardExpected("fold along ");
	int axis = PuzzleInput::GetChar();
	int32_t line = Parse::GetInt32();

	FoldDots(dotBuffers[0], &dotBuffers[1], static_cast<char>(axis), line);

	int32_t answer = dotBuffers[1].Size();

	PuzzleOutput::Submit(2021, 13, 1, answer);
}

void Puzzle13_B_2021()
{
	const uint32_t dotHashSize = 2 * 1024;

	array<HashSet<Vec2Int>, 2> dotBuffers
	{
		HashSet<Vec2Int>(dotHashSize, Vec2Int::Min()),
		HashSet<Vec2Int>(dotHashSize, Vec2Int::Min()),
	};

	ReadDots(&dotBuffers[0]);

	size_t activeBuffer = 0;
	while (PuzzleInput::NextLine())
	{
		Parse::DiscardExpected("fold along ");
		int axis = PuzzleInput::GetChar();
		int32_t line = Parse::GetInt32();

		FoldDots(dotBuffers[activeBuffer & 1], &dotBuffers[(activeBuffer + 1) & 1], static_cast<char>(axis), line);

		activeBuffer++;
	}

	string answer = PrintPuzzle(dotBuffers[activeBuffer & 1]);

	PuzzleOutput::Submit(2021, 13, 2, answer.c_str());
}

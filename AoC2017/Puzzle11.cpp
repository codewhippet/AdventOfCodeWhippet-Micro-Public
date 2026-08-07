#include "stdafx.h"

using namespace std;
using namespace HexFlat;

namespace Puzzle11_2017_Types
{
}

using namespace Puzzle11_2017_Types;

void Puzzle11_A_2017()
{
	// | State | Input | Next State |
	// |-------|-------|------------|
	// | 0     | n     | 1          | "n"
	// | 1     | e     | 2          | "ne"
	// | 1     | w     | 3          | "nw"
	// | 0     | s     | 4          | "s"
	// | 4     | e     | 5          | "se"
	// | 4     | w     | 6          | "sw"
	// | *     | .     | 0          | Move

	const array<HexInt, 7> hexIndices =
	{
		HexInt{},
		HexInt::North(),
		HexInt::NorthEast(),
		HexInt::NorthWest(),
		HexInt::South(),
		HexInt::SouthEast(),
		HexInt::SouthWest(),
	};

	const array<size_t, 7> nTransitions = { 1, 0, 0, 0, 0, 0, 0 };
	const array<size_t, 7> eTransitions = { 0, 2, 0, 0, 5, 0, 0 };
	const array<size_t, 7> wTransitions = { 0, 3, 0, 0, 6, 0, 0 };
	const array<size_t, 7> sTransitions = { 4, 0, 0, 0, 0, 0, 0 };

	HexInt currentPosition;

	size_t state = 0;
	while (true)
	{
		int c = PuzzleInput::GetChar();
		switch (c)
		{
		case 'n':
			state = nTransitions[state];
			break;

		case 'e':
			state = eTransitions[state];
			break;

		case 'w':
			state = wTransitions[state];
			break;

		case 's':
			state = sTransitions[state];
			break;

		default:
			currentPosition = currentPosition + hexIndices[state];
			state = 0;
			break;
		}

		if (c == '\n')
			break;
	}

	int32_t answer = MahattanDistance(currentPosition, HexInt{});
	PuzzleOutput::Submit(2017, 11, 1, answer);
}

void Puzzle11_B_2017()
{
	// | State | Input | Next State |
	// |-------|-------|------------|
	// | 0     | n     | 1          | "n"
	// | 1     | e     | 2          | "ne"
	// | 1     | w     | 3          | "nw"
	// | 0     | s     | 4          | "s"
	// | 4     | e     | 5          | "se"
	// | 4     | w     | 6          | "sw"
	// | *     | .     | 0          | Move

	const array<HexInt, 7> hexIndices =
	{
		HexInt{},
		HexInt::North(),
		HexInt::NorthEast(),
		HexInt::NorthWest(),
		HexInt::South(),
		HexInt::SouthEast(),
		HexInt::SouthWest(),
	};

	const array<size_t, 7> nTransitions = { 1, 0, 0, 0, 0, 0, 0 };
	const array<size_t, 7> eTransitions = { 0, 2, 0, 0, 5, 0, 0 };
	const array<size_t, 7> wTransitions = { 0, 3, 0, 0, 6, 0, 0 };
	const array<size_t, 7> sTransitions = { 4, 0, 0, 0, 0, 0, 0 };

	HexInt currentPosition;
	int32_t answer = 0;

	size_t state = 0;
	while (true)
	{
		int c = PuzzleInput::GetChar();
		switch (c)
		{
		case 'n':
			state = nTransitions[state];
			break;

		case 'e':
			state = eTransitions[state];
			break;

		case 'w':
			state = wTransitions[state];
			break;

		case 's':
			state = sTransitions[state];
			break;

		default:
			currentPosition = currentPosition + hexIndices[state];
			answer = max(answer, MahattanDistance(currentPosition, HexInt{}));
			state = 0;
			break;
		}

		if (c == '\n')
			break;
	}

	PuzzleOutput::Submit(2017, 11, 2, answer);
}

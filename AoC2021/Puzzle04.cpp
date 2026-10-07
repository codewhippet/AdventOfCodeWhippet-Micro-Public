#include "stdafx.h"

using namespace std;

namespace Puzzle04_2021_Types
{
	enum : size_t
	{
		CARD_SIZE = 5
	};

	struct NumberLocation
	{
		int32_t Board;
		int32_t Row;
		int32_t Column;
	};

	struct BoardState
	{
		int32_t HasBingo = 0;
		array<int32_t, CARD_SIZE> RowCounts = {};
		array<int32_t, CARD_SIZE> ColumnCounts = {};
	};
}

using namespace Puzzle04_2021_Types;

void Puzzle04_A_2021()
{
	const size_t expectedRandomNumbers = 100;
	const size_t numNumbers = 100;
	const size_t numBoards = 100;

	vector<int32_t> randomNumbers;
	randomNumbers.reserve(expectedRandomNumbers);
	while (PuzzleInput::PeekChar() != '\n')
	{
		randomNumbers.push_back(Parse::GetInt32());
	}

	PuzzleInput::NextLine();

	vector<SmallVector<NumberLocation, 40>> numberLocations(numNumbers);
	vector<SmallVector<int32_t, CARD_SIZE * CARD_SIZE>> boardNumbers(numBoards);
	for (int32_t board = 0; PuzzleInput::PeekChar() != EOF; board++)
	{
		for (int32_t row = 0; row < CARD_SIZE; row++)
		{
			for (int32_t column = 0; column < CARD_SIZE; column++)
			{
				int32_t value = Parse::GetInt32();
				numberLocations[value].PushBack({ board, row, column });
				boardNumbers[board].PushBack(value);
			}
		}

		PuzzleInput::NextLine();
	}

	int32_t answer = -1;

	vector<int32_t> numberScores(numNumbers, 1);

	vector<BoardState> boards(numBoards);
	for (int32_t call : randomNumbers)
	{
		numberScores[call] = 0;

		for (const NumberLocation& location : numberLocations[call])
		{
			BoardState& board = boards[location.Board];
			int32_t rowCount = ++board.RowCounts[location.Row];
			int32_t columnCount = ++board.ColumnCounts[location.Column];
			if ((rowCount == 5) || (columnCount == 5))
			{
				int32_t uncheckedSum = 0;
				for (int32_t number : boardNumbers[location.Board])
				{
					uncheckedSum += number * numberScores[number];
				}

				answer = uncheckedSum * call;
				break;
			}
		}

		if (answer != -1)
			break;
	}
	
	PuzzleOutput::Submit(2021, 4, 1, answer);
}

void Puzzle04_B_2021()
{
	const size_t expectedRandomNumbers = 100;
	const size_t numNumbers = 100;
	const size_t numBoards = 100;

	vector<int32_t> randomNumbers;
	randomNumbers.reserve(expectedRandomNumbers);
	while (PuzzleInput::PeekChar() != '\n')
	{
		randomNumbers.push_back(Parse::GetInt32());
	}

	PuzzleInput::NextLine();

	vector<SmallVector<NumberLocation, 40>> numberLocations(numNumbers);
	vector<SmallVector<int32_t, CARD_SIZE * CARD_SIZE>> boardNumbers(numBoards);
	for (int32_t board = 0; PuzzleInput::PeekChar() != EOF; board++)
	{
		for (int32_t row = 0; row < CARD_SIZE; row++)
		{
			for (int32_t column = 0; column < CARD_SIZE; column++)
			{
				int32_t value = Parse::GetInt32();
				numberLocations[value].PushBack({ board, row, column });
				boardNumbers[board].PushBack(value);
			}
		}

		PuzzleInput::NextLine();
	}

	int32_t answer = -1;

	vector<int32_t> numberScores(numNumbers, 1);

	int32_t bingoCount = 0;

	vector<BoardState> boards(numBoards);
	for (int32_t call : randomNumbers)
	{
		numberScores[call] = 0;

		for (const NumberLocation& location : numberLocations[call])
		{
			BoardState& board = boards[location.Board];
			int32_t rowCount = ++board.RowCounts[location.Row];
			int32_t columnCount = ++board.ColumnCounts[location.Column];
			bool bingoNow = (rowCount == 5) || (columnCount == 5);
			bool newBingo = bingoNow && !board.HasBingo;
			if (newBingo)
			{
				board.HasBingo = true;
				if (++bingoCount == numBoards)
				{
					int32_t uncheckedSum = 0;
					for (int32_t number : boardNumbers[location.Board])
					{
						uncheckedSum += number * numberScores[number];
					}

					answer = uncheckedSum * call;
					break;
				}
			}
		}

		if (answer != -1)
			break;
	}

	PuzzleOutput::Submit(2021, 4, 2, answer);
}

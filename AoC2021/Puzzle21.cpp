#include "stdafx.h"

using namespace std;

namespace Puzzle21_2021_Types
{
	enum : size_t
	{
		NumPlayers = 2,
		NumPositions = 10,
		ScoreToWin = 21,
		MaxTurns = ScoreToWin,
	};

	struct Game
	{
		// Using way more memory than we need to, but it's handy to have a history for debugging
		int64_t Universes[MaxTurns][NumPlayers][NumPositions][ScoreToWin] = {};
	};
}

using namespace Puzzle21_2021_Types;

static int32_t Roll(int32_t* dice)
{
	int32_t roll = 0;

	roll += ++(*dice);
	*dice %= 100;
	roll += ++(*dice);
	*dice %= 100;
	roll += ++(*dice);
	*dice %= 100;

	return roll;
}

void Puzzle21_A_2021()
{
	array<char, 32> line;
	Parse::ReadNonEmptyLine(line.data(), line.size());
	const int32_t p1StartingPosition = line[28] - '0' - 1;
	Parse::ReadNonEmptyLine(line.data(), line.size());
	const int32_t p2StartingPosition = line[28] - '0' - 1;

	array<int32_t, 2> positions{ p1StartingPosition, p2StartingPosition };
	array<int32_t, 2> scores = { 0, 0 };

	const int32_t scoreToWin = 1000;

	int32_t numRolls = 0;
	int32_t currentPlayer = 0;
	int32_t dice = 0;
	while (true)
	{
		numRolls += 3;

		positions[currentPlayer] = (positions[currentPlayer] + Roll(&dice)) % 10;
		scores[currentPlayer] += positions[currentPlayer] + 1;

		if (scores[currentPlayer] >= scoreToWin)
			break;

		currentPlayer = 1 - currentPlayer;
	}

	int32_t answer = numRolls * scores[1 - currentPlayer];
	PuzzleOutput::Submit(2021, 21, 1, answer);
}

void Puzzle21_B_2021()
{
	array<char, 32> line;
	Parse::ReadNonEmptyLine(line.data(), line.size());
	const int32_t p1StartingPosition = line[28] - '0' - 1;
	Parse::ReadNonEmptyLine(line.data(), line.size());
	const int32_t p2StartingPosition = line[28] - '0' - 1;

	Game *game = new Game();
	game->Universes[0][0][p1StartingPosition][0] = 1;
	game->Universes[1][1][p2StartingPosition][0] = 1;
	Game& g = *game;

	const vector<pair<int32_t, int32_t>> rolls =
	{
		// X universes in which we roll Y
		{ 1, 3 },
		{ 3, 4 },
		{ 6, 5 },
		{ 7, 6 },
		{ 6, 7 },
		{ 3, 8 },
		{ 1, 9 },
	};

	array<int64_t, 2> playerWins = {};
	int64_t otherPlayerActiveUniverses = 1;
	for (int32_t turn = 0; turn < MaxTurns; turn++)
	{
		int currentPlayer = turn & 1;

		int64_t nextActiveUniverses = 0;
		for (uint32_t position = 0; position < NumPositions; position++)
		{
			for (uint32_t score = 0; score < ScoreToWin; score++)
			{
				int64_t playerUniverses = g.Universes[turn][currentPlayer][position][score];
				if (playerUniverses == 0)
					continue;

				for (const auto& roll : rolls)
				{
					int64_t nextPosition = (position + roll.second) % 10;
					int64_t nextScore = score + nextPosition + 1;
					if (nextScore >= ScoreToWin)
					{
						playerWins[currentPlayer] += playerUniverses * roll.first * otherPlayerActiveUniverses;
					}
					else
					{
						g.Universes[turn + 2][currentPlayer][nextPosition][nextScore] += playerUniverses * roll.first;
						nextActiveUniverses += playerUniverses * roll.first;
					}
				}
			}
		}

		otherPlayerActiveUniverses = nextActiveUniverses;
		if (otherPlayerActiveUniverses == 0)
			break;
	}

	delete game;

	int64_t answer = max(playerWins[0], playerWins[1]);
	PuzzleOutput::Submit(2021, 21, 2, answer);
}

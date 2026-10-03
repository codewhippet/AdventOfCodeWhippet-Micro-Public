#include "stdafx.h"

using namespace std;

namespace Puzzle22_2020_Types
{
	class Hand
	{
	public:
		Hand() = default;

		Hand(const Hand& other, int32_t numToCopy)
		{
			for (int32_t i = 0; i < numToCopy; i++)
			{
				CardsNew[End++ & 63] = other.CardsNew[(other.Begin + i) & 63];
			}
		}

		bool IsEmpty() const
		{
			return (Begin == End);
		}

		int32_t NumCards() const
		{
			return (End - Begin);
		}

		int32_t GetFrontCard()
		{
			int32_t card = CardsNew[Begin++ & 63];
			return card;
		}

		void InsertCardAtBack(int32_t card)
		{
			CardsNew[End++ & 63] = card;
		}

		int32_t Score() const
		{
			int32_t score = 0;

			int32_t numCards = End - Begin;
			for (int32_t i = 0; i < (End - Begin); i++)
			{
				int32_t card = CardsNew[(Begin + i) & 63];
				score += card * numCards;
				numCards--;
			}

			return score;
		}

		bool operator==(const Hand& other) const
		{
			if ((End - Begin) != (other.End - other.Begin))
			{
				return false;
			}

			int32_t numCards = End - Begin;
			for (int32_t i = 0; i < numCards; i++)
			{
				if (CardsNew[(Begin + i) & 63] != other.CardsNew[(other.Begin + i) & 63])
				{
					return false;
				}
			}

			return true;
		}

	private:
		array<int32_t, 64> CardsNew;
		int32_t Begin = 0;
		int32_t End = 0;
	};

	struct WinHistory
	{
		WinHistory()
			: HistoryBits(12 * 1024 / 8)
		{
		}

		void Set(int32_t round, int32_t winner)
		{
			assert((winner == 0) || (winner == 1));

			int32_t chunk = round >> 5;
			int32_t bitIndex = round & (32 - 1);
			uint32_t bit = winner << bitIndex;

			HistoryBits[chunk] = (HistoryBits[chunk] & ~bit) | bit;
		}

		int32_t Get(int32_t round)
		{
			int32_t chunk = round >> 5;
			int32_t bitIndex = round & (32 - 1);
			uint32_t bit = 1 << bitIndex;

			return (HistoryBits[chunk] & bit ? 1 : 0);
		}

		vector<uint32_t> HistoryBits;
	};

	struct ExecutionState
	{
		array<Hand, 2> PlayerHands;
		array<int32_t, 2> PlayerCards;
		int32_t RoundWinner = -1;

		int32_t HareRound = 0;
		int32_t TortoiseRound = 0;
		array<Hand, 2> TortoiseHands;
		WinHistory Winners;

		int32_t *Return = nullptr;
	};
}

using namespace Puzzle22_2020_Types;

static Hand ReadPlayerHand()
{
	Hand ret;

	PuzzleInput::DropLine();
	PuzzleInput::NextLine();

	while (isdigit(PuzzleInput::PeekChar()))
	{
		ret.InsertCardAtBack(Parse::GetInt32());
		PuzzleInput::NextLine();
	}

	return ret;
}

static int32_t PlayGame(const array<Hand, 2>& startingHands, Hand* winningHand)
{
	vector<ExecutionState> exec;
	exec.reserve(8);

	int32_t overallWinner = -1;	

	exec.push_back({});
	exec.back().PlayerHands = startingHands;
	exec.back().TortoiseHands = exec.back().PlayerHands;
	exec.back().Return = &overallWinner;

	while (exec.empty() == false)
	{
		ExecutionState& current = exec.back();

		if (current.RoundWinner != -1)
		{
			current.PlayerHands[current.RoundWinner].InsertCardAtBack(current.PlayerCards[current.RoundWinner]);
			current.PlayerHands[current.RoundWinner].InsertCardAtBack(current.PlayerCards[1 - current.RoundWinner]);

			current.Winners.Set(current.HareRound++, current.RoundWinner);
		}

		if (current.PlayerHands[0].IsEmpty())
		{
			if (exec.size() == 1)
			{
				*winningHand = current.PlayerHands[1];
			}

			*current.Return = 1;
			exec.pop_back();
			continue;
		}

		if (current.PlayerHands[1].IsEmpty())
		{
			if (exec.size() == 1)
			{
				*winningHand = current.PlayerHands[0];
			}

			*current.Return = 0;
			exec.pop_back();
			continue;
		}

		// Stop infinite games
		if (current.HareRound > 0)
		{
			assert(current.HareRound > current.TortoiseRound);

			if (current.PlayerHands == current.TortoiseHands)
			{
				*current.Return = 0;
				exec.pop_back();
				continue;
			}

			if (current.HareRound & 1)
			{
				array<int32_t, 2> tortoiseCards;
				tortoiseCards[0] = current.TortoiseHands[0].GetFrontCard();
				tortoiseCards[1] = current.TortoiseHands[1].GetFrontCard();
				assert(tortoiseCards[0] != tortoiseCards[1]);

				int32_t previousWinner = current.Winners.Get(current.TortoiseRound);

				current.TortoiseHands[previousWinner].InsertCardAtBack(tortoiseCards[previousWinner]);
				current.TortoiseHands[previousWinner].InsertCardAtBack(tortoiseCards[1 - previousWinner]);

				current.TortoiseRound++;
			}
		}

		// Draw
		current.PlayerCards[0] = current.PlayerHands[0].GetFrontCard();
		current.PlayerCards[1] = current.PlayerHands[1].GetFrontCard();
		assert(current.PlayerCards[0] != current.PlayerCards[1]);

		current.RoundWinner = -1;

		// Should we recurse?
		if ((current.PlayerHands[0].NumCards() >= current.PlayerCards[0]) &&
			(current.PlayerHands[1].NumCards() >= current.PlayerCards[1]))
		{
			exec.push_back({});
			exec.back().PlayerHands[0] = Hand(current.PlayerHands[0], current.PlayerCards[0]);
			exec.back().PlayerHands[1] = Hand(current.PlayerHands[1], current.PlayerCards[1]);
			exec.back().TortoiseHands = exec.back().PlayerHands;
			exec.back().Return = &current.RoundWinner;
		}
		else
		{
			current.RoundWinner = (current.PlayerCards[0] > current.PlayerCards[1] ? 0 : 1);
		}
	}

	return overallWinner;
}

void Puzzle22_A_2020()
{
	array<Hand, 2> playerHands;
	playerHands[0] = ReadPlayerHand();
	playerHands[1] = ReadPlayerHand();

	while ((playerHands[0].IsEmpty() == false) && (playerHands[1].IsEmpty() == false))
	{
		int32_t player1Card = playerHands[0].GetFrontCard();
		int32_t player2Card = playerHands[1].GetFrontCard();
		assert(player1Card != player2Card);

		if (player1Card > player2Card)
		{
			playerHands[0].InsertCardAtBack(player1Card);
			playerHands[0].InsertCardAtBack(player2Card);
		}
		else
		{
			playerHands[1].InsertCardAtBack(player2Card);
			playerHands[1].InsertCardAtBack(player1Card);
		}
	}

	int32_t winnerIndex = (playerHands[0].IsEmpty() ? 1 : 0);
	int32_t answer = playerHands[winnerIndex].Score();

	PuzzleOutput::Submit(2020, 22, 1, answer);
}

void Puzzle22_B_2020()
{
	array<Hand, 2> startingHands;
	startingHands[0] = ReadPlayerHand();
	startingHands[1] = ReadPlayerHand();

	Hand winningHand;
	PlayGame(startingHands, &winningHand);

	int32_t answer = winningHand.Score();

	PuzzleOutput::Submit(2020, 22, 2, answer);
}

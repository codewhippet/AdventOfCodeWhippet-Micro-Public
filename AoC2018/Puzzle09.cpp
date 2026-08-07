#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle09_2018_Types
{
	struct PackedMarble
	{
		uint8_t Lo;
		uint8_t Mid;
		uint8_t Hi;
	};
}

using namespace Puzzle09_2018_Types;

static PackedMarble Pack(uint32_t marble)
{
	return { static_cast<uint8_t>(marble), static_cast<uint8_t>(marble >> 8), static_cast<uint8_t>(marble >> 16) };
}

static uint32_t Unpack(const PackedMarble& marble)
{
	return marble.Lo | (marble.Mid << 8) | (marble.Hi << 16);
}

static uint32_t Wrap(uint32_t index, uint32_t bufferSize)
{
	assert(index < (bufferSize * 2));
	if (index >= bufferSize)
		index -= bufferSize;
	return index;
}

static uint32_t FindMaximumScore(int32_t numPlayers, uint32_t lastMarble, PackedMarble* marbleBuffer, uint32_t bufferSize)
{
	vector<uint32_t> playerScores(numPlayers);

	uint32_t tail = 0;
	uint32_t head = 1;

	uint32_t marble = 0;
	uint32_t phase = 0;

	uint32_t currentPlayer = numeric_limits<uint32_t>::max();
	uint32_t lastSevenMarble = 0;

	while ((marble < lastMarble) && (Wrap(head, bufferSize) != Wrap(tail, bufferSize)))
	{
		if (++currentPlayer == static_cast<int32_t>(playerScores.size()))
			currentPlayer = 0;

		marble++;
		phase++;

		if (phase == 19)
		{
			lastSevenMarble = Unpack(marbleBuffer[Wrap(tail++, bufferSize)]);
			marbleBuffer[Wrap(head++, bufferSize)] = Pack(marble);
		}
		else if (phase == 23)
		{
			tail -= 6;
			head -= 6;

			for (uint32_t i = 0; i < 6; i++)
			{
				marbleBuffer[Wrap(tail + i, bufferSize)] = marbleBuffer[Wrap(head + i, bufferSize)];

			}

			playerScores[currentPlayer] += marble + lastSevenMarble;

			phase = 0;
		}
		else
		{
			marbleBuffer[Wrap(head++, bufferSize)] = marbleBuffer[Wrap(tail++, bufferSize)];
			marbleBuffer[Wrap(head++, bufferSize)] = Pack(marble);
		}
	}

	while ((marble < lastMarble) && (tail != head))
	{
		if (++currentPlayer == static_cast<int32_t>(playerScores.size()))
			currentPlayer = 0;

		marble++;
		phase++;

		if (phase == 19)
		{
			lastSevenMarble = Unpack(marbleBuffer[Wrap(tail++, bufferSize)]);
		}
		else if (phase == 23)
		{
			tail -= 6;

			playerScores[currentPlayer] += marble + lastSevenMarble;

			phase = 0;
		}
		else
		{
			tail++;
		}
	}

	uint32_t answer = ranges::max(playerScores);

	return answer;
}

void Puzzle09_A_2018()
{
	uint32_t numPlayers = Parse::GetUint32();
	uint32_t lastMarble = Parse::GetUint32();

	vector<PackedMarble> marbleBuffer((192 * 1024) / sizeof(PackedMarble));
	uint32_t answer = FindMaximumScore(numPlayers, lastMarble, marbleBuffer.data(), static_cast<uint32_t>(marbleBuffer.size()));

	PuzzleOutput::Submit(2018, 9, 1, static_cast<size_t>(answer));
}

void Puzzle09_B_2018()
{
	const size_t psramNeeded = 8 * 1024 * 1024;
	if (Hardware::PsramSize() >= psramNeeded)
	{
		uint32_t numPlayers = Parse::GetUint32();
		uint32_t lastMarble = Parse::GetUint32();

		uint32_t answer = FindMaximumScore(numPlayers,
			lastMarble * 1000,
			static_cast<PackedMarble*>(Hardware::PsramBase()),
			static_cast<uint32_t>(Hardware::PsramSize() / sizeof(PackedMarble)));

		PuzzleOutput::Submit(2018, 9, 2, static_cast<size_t>(answer));
	}
	else
	{
		PuzzleOutput::Submit(2018, 9, 2, "[UNSUPPORTED]");
	}
}

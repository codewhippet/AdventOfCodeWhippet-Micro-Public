#include "stdafx.h"

using namespace std;

namespace Puzzle16_2017_Types
{
	using Programs = array<char, 16>;

	struct DanceMove
	{
		char Type;
		uint8_t A;
		uint8_t B;
	};
}

using namespace Puzzle16_2017_Types;

template <>
struct std::hash<Programs>
{
	size_t operator()(const Programs& p) const noexcept
	{
		return hash<string_view>{}(string_view(p.data(), p.size()));
	}
};

static vector<DanceMove> ParseDanceMoves()
{
	vector<DanceMove> moves;
	moves.reserve(10000);

	while (true)
	{
		int c = PuzzleInput::GetChar();
		if (c == EOF)
			break;

		switch (c)
		{
		case 's':
			moves.push_back({ 's', Parse::GetUint8(), 0 });
			break;
		case 'x':
			moves.push_back({ 'x', Parse::GetUint8(), Parse::GetUint8() });
			break;
		case 'p':
			uint8_t a = static_cast<uint8_t>(PuzzleInput::GetChar());
			PuzzleInput::DropChar();
			uint8_t b = static_cast<uint8_t>(PuzzleInput::GetChar());
			moves.push_back({ 'p', a, b });
			break;
		}

		PuzzleInput::DropChar();
	}

	return moves;
}

static Programs CreatePrograms()
{
	Programs programs;
	iota(programs.begin(), programs.end(), 'a');
	return programs;
}

static void Spin(Programs* oldPrograms, uint8_t spin)
{
	Programs newPrograms;
	for (size_t i = 0; i < newPrograms.size(); i++)
	{
		size_t sourceIndex = (i + oldPrograms->size() - spin) & (oldPrograms->size() - 1);
		newPrograms[i] = (*oldPrograms)[sourceIndex];
	}
	*oldPrograms = newPrograms;
}

static void Exchange(Programs* programs, uint8_t positionA, uint8_t positionB)
{
	swap((*programs)[positionA], (*programs)[positionB]);
}

static void Partner(Programs* programs, uint8_t programA, uint8_t programB)
{
	for (size_t i = 0; i < programs->size(); i++)
	{
		if ((*programs)[i] == programA)
		{
			(*programs)[i] = programB;
		}
		else if ((*programs)[i] == programB)
		{
			(*programs)[i] = programA;
		}
	}
}

static Programs Dance(Programs programs, const vector<DanceMove>& moves)
{
	for (const DanceMove& move : moves)
	{
		switch (move.Type)
		{
		case 's':
			Spin(&programs, move.A);
			break;
		case 'x':
			Exchange(&programs, move.A, move.B);
			break;
		case 'p':
			Partner(&programs, move.A, move.B);
			break;
		}
	}

	return programs;
}

void Puzzle16_A_2017()
{
	vector<DanceMove> moves = ParseDanceMoves();
	Programs programs = CreatePrograms();
	Programs dancedPrograms = Dance(programs, moves);

	string answer(dancedPrograms.data(), dancedPrograms.size());
	PuzzleOutput::Submit(2017, 16, 1, answer.c_str());
}

void Puzzle16_B_2017()
{
	vector<DanceMove> moves = ParseDanceMoves();
	Programs programs = CreatePrograms();

	HashMap<Programs, size_t> states(128, {});

	size_t loopBegin = numeric_limits<size_t>::max();
	size_t loopEnd = numeric_limits<size_t>::max();

	size_t iterations = 0;
	while (true)
	{
		states.Insert(programs, iterations++);

		programs = Dance(programs, moves);

		if (states.TryFind(programs, &loopBegin))
		{
			loopEnd = iterations;
			break;
		}
	}

	size_t loopSize = loopEnd - loopBegin;

	size_t loopRemainder = (1000000000 - loopBegin) % loopSize;
	size_t iterationsNeeded = loopBegin + loopRemainder;

	Programs finalArrangement = ranges::find_if(states, [&](const auto& kvp) { return kvp.second == iterationsNeeded; })->first;

	string answer(finalArrangement.data(), finalArrangement.size());
	PuzzleOutput::Submit(2017, 16, 2, answer.c_str());
}

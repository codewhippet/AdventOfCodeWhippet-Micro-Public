#include "stdafx.h"

using namespace std;

namespace Puzzle25_2017_Types
{
	struct Instruction
	{
		int32_t Write;
		int32_t Move;
		size_t NextState;
	};

	struct State
	{
		array<Instruction, 2> Instructions;
	};

	struct Puzzle
	{
		size_t StartingState;
		uint32_t StepsToExecute;
		vector<State> StateMachine;
	};
}

using namespace Puzzle25_2017_Types;

static void ParseHeader(Puzzle* p)
{
	char line[64];

	Parse::ReadNonEmptyLine(line);
	p->StartingState = line[15] - 'A'; // Begin in state [A..Z].

	Parse::ReadNonEmptyLine(line);
	p->StepsToExecute = Parse::GetUint32(line); // Perform a diagnostic checksum after N steps.

	PuzzleInput::DropLine();
}

static void ParseState(Puzzle* p)
{
	char line[64];

	State state;
	PuzzleInput::DropLine(); // In state A:
	PuzzleInput::NextLine();

	for (size_t i = 0; i < 2; i++)
	{
		PuzzleInput::DropLine(); // If the current value is [0|1]:
		PuzzleInput::NextLine();

		Parse::ReadNonEmptyLine(line);
		state.Instructions[i].Write = Parse::GetInt32(line); // - Write the value N.

		Parse::ReadNonEmptyLine(line);
		state.Instructions[i].Move = (line[27] == 'r' ? 1 : -1); // - Move one slot to the [right|left].

		Parse::ReadNonEmptyLine(line);
		state.Instructions[i].NextState = line[26] - 'A'; // - Continue with state [A..Z].
	}

	PuzzleInput::DropLine();
	PuzzleInput::NextLine();

	p->StateMachine.push_back(state);
}

static void ParsePuzzle(Puzzle* p)
{
	ParseHeader(p);
	p->StateMachine.reserve(8);

	while (PuzzleInput::NextLine())
	{
		ParseState(p);
	}
}

void Puzzle25_A_2017()
{
	Puzzle p;
	ParsePuzzle(&p);

	vector<int32_t> tape(5 * 1024);
	size_t head = 4 * 1024;

	size_t currentState = p.StartingState;
	for (uint32_t i = 0; i < p.StepsToExecute; i++)
	{
		int32_t currentValue = tape[head];
		const Instruction& inst = p.StateMachine[currentState].Instructions[currentValue];
		tape[head] = inst.Write;
		head += inst.Move;
		currentState = inst.NextState;
	}

	int32_t answer = static_cast<int32_t>(ranges::count(tape, 1));

	PuzzleOutput::Submit(2017, 25, 1, answer);
}

void Puzzle25_B_2017()
{
	return PuzzleOutput::Submit(2017, 25, 2, int32_t(-1));
}

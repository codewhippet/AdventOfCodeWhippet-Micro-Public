#include "stdafx.h"

using namespace std;

namespace Puzzle08_2020_Types
{
	enum class Inst : int32_t
	{
		Nop,
		Acc,
		Jmp,
	};
}

using namespace Puzzle08_2020_Types;

static vector<pair<Inst, int32_t>> ReadProgram()
{
	vector<pair<Inst, int32_t>> program;
	program.reserve(1024);

	while (PuzzleInput::NextLine())
	{
		switch (PuzzleInput::GetChar())
		{
		case 'n':
			program.push_back({ Inst::Nop, Parse::GetInt32() });
			break;
		case 'a':
			program.push_back({ Inst::Acc, Parse::GetInt32() });
			break;
		case 'j':
			program.push_back({ Inst::Jmp, Parse::GetInt32() });
			break;
		}
	}

	return program;
}

static bool RunsToCompletion(const vector<pair<Inst, int32_t>>& program, int32_t* accOut)
{
	vector<int32_t> executed(program.size());

	int32_t acc = 0;
	size_t pc = 0;

	while (true)
	{
		if (pc == program.size())
		{
			*accOut = acc;
			return true;
		}

		if (executed[pc])
		{
			*accOut = acc;
			return false;
		}

		assert(pc < program.size());

		executed[pc] = 1;

		switch (program[pc].first)
		{
			case Inst::Nop:
			pc++;
			break;

		case Inst::Acc:
			acc += program[pc].second;
			pc++;
			break;

		case Inst::Jmp:
			pc += program[pc].second;
			break;
		}
	};
}

void Puzzle08_A_2020()
{
	vector<pair<Inst, int32_t>> program = ReadProgram();

	int32_t acc = 0;
	bool shouldFail = RunsToCompletion(program, &acc);
	assert(shouldFail == false);
	(void)shouldFail;

	int32_t answer = acc;

	PuzzleOutput::Submit(2020, 8, 1, answer);
}

void Puzzle08_B_2020()
{
	vector<pair<Inst, int32_t>> program = ReadProgram();

	int32_t answer = 0;
	for (size_t i = 0; i < program.size(); i++)
	{
		if (program[i].first == Inst::Acc)
			continue;

		Inst stash;
		if (program[i].first == Inst::Nop)
		{
			program[i].first = Inst::Jmp;
			stash = Inst::Nop;
		}
		else
		{
			assert(program[i].first == Inst::Jmp);
			program[i].first = Inst::Nop;
			stash = Inst::Jmp;
		}

		int32_t acc;
		if (RunsToCompletion(program, &acc))
		{
			answer = acc;
			break;
		}

		program[i].first = stash;
	}

	PuzzleOutput::Submit(2020, 8, 2, answer);
}

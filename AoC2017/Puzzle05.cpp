#include "stdafx.h"

using namespace std;

namespace Puzzle05_2017_Types
{
}

using namespace Puzzle05_2017_Types;

static vector<int32_t> ReadInstructions()
{
	vector<int32_t> instructions;
	instructions.reserve(2048);

	while (PuzzleInput::NextLine())
	{
		instructions.push_back(Parse::GetInt32());
	}

	return instructions;
}

void Puzzle05_A_2017()
{
	vector<int32_t> instructions = ReadInstructions();

	size_t pc = 0;

	int32_t answer = 0;
	while (true)
	{
		pc = pc + instructions[pc]++;
		answer++;

		if (pc >= instructions.size())
			break;
	}

	PuzzleOutput::Submit(2017, 5, 1, answer);
}

void Puzzle05_B_2017()
{
	vector<int32_t> instructions = ReadInstructions();

	size_t pc = 0;

	int32_t answer = 0;
	while (true)
	{
		int32_t jump = instructions[pc];
		size_t nextPc = pc + jump;
		instructions[pc] += (jump >= 3 ? -1 : 1);
		pc = nextPc;
		answer++;

		if (pc >= instructions.size())
			break;
	}

	PuzzleOutput::Submit(2017, 5, 2, answer);
}

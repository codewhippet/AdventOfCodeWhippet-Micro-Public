#include "stdafx.h"

using namespace std;

namespace Puzzle02_2019_Types
{
}

using namespace Puzzle02_2019_Types;

static void Execute(vector<int32_t>* program)
{
	vector<int32_t>& p = *program;

	int32_t pc = 0;
	while (true)
	{
		int32_t instruction = p[pc];
		switch (instruction)
		{
		case 99:
			return;

		case 1:
			{
				int32_t a = p[p[pc + 1]];
				int32_t b = p[p[pc + 2]];
				p[p[pc + 3]] = a + b;
				pc += 4;
			}
			break;

		case 2:
			{
				int32_t a = p[p[pc + 1]];
				int32_t b = p[p[pc + 2]];
				p[p[pc + 3]] = a * b;
				pc += 4;
			}
			break;
		}
	}
}

static int32_t GravityAssist(vector<int32_t> program, int32_t a, int32_t b)
{
	program[1] = a;
	program[2] = b;
	Execute(&program);
	return program[0];
}

static int32_t FindGravityAssist(const vector<int32_t>& program)
{
	for (int32_t noun : views::iota(0, 100))
	{
		for (int32_t verb : views::iota(0, 100))
		{
			if (GravityAssist(program, noun, verb) == 19690720)
			{
				return 100 * noun + verb;
			}
		}
	}
	return -1;
}

void Puzzle02_A_2019()
{
	vector<int32_t> program;
	program.reserve(200);
	while (PuzzleInput::NextLine())
	{
		program.push_back(Parse::GetInt32());
	}

	program[1] = 12;
	program[2] = 2;

	Execute(&program);

	int32_t answer = program[0];
	PuzzleOutput::Submit(2019, 2, 1, answer);
}

void Puzzle02_B_2019()
{
	vector<int32_t> program;
	program.reserve(200);
	while (PuzzleInput::NextLine())
	{
		program.push_back(Parse::GetInt32());
	}

	int32_t answer = FindGravityAssist(program);

	PuzzleOutput::Submit(2019, 2, 1, answer);
}

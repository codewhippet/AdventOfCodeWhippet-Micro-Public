#include "stdafx.h"

using namespace std;

namespace Puzzle12_2016_Types
{
	enum class Opcode
	{
		CpyImm,
		Cpy,
		Inc,
		Dec,
		JnzImm,
		Jnz,
	};

	struct Instruction
	{
		Opcode Op;
		int32_t A = 0;
		int32_t B = 0;
	};

	struct Computer
	{
		size_t PC = 0;
		int32_t Registers[4] = { 0 };
	};
}

using namespace Puzzle12_2016_Types;

static vector<Instruction> ParseProgram()
{
	vector<Instruction> program;
	program.reserve(32);

	auto Register = [](char c) -> int32_t { return c - 'a'; };

	array<char, 16> lineBuffer;
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(lineBuffer.data(), lineBuffer.size());
		string_view line(lineBuffer.data());

		Instruction inst;
		{
			char paramA[16] = { 0 };
			char paramB[16] = { 0 };

			if (line.starts_with("cpy"sv))
			{
				int scanned = sscanf(line.data(), "cpy %s %s",
					paramA,
					paramB);
				assert(scanned == 2);
				(void)scanned;

				if (isdigit(paramA[0]))
				{
					inst.Op = Opcode::CpyImm;
					inst.A = atoi(paramA);
				}
				else
				{
					inst.Op = Opcode::Cpy;
					inst.A = Register(paramA[0]);
				}
				inst.B = Register(paramB[0]);
			}
			else if (line.starts_with("inc"sv))
			{
				int scanned = sscanf(line.data(), "inc %s",
					paramA);
				assert(scanned == 1);
				(void)scanned;

				inst.Op = Opcode::Inc;
				inst.A = Register(paramA[0]);
			}
			else if (line.starts_with("dec"sv))
			{
				int scanned = sscanf(line.data(), "dec %s",
					paramA);
				assert(scanned == 1);
				(void)scanned;

				inst.Op = Opcode::Dec;
				inst.A = Register(paramA[0]);
			}
			else if (line.starts_with("jnz"sv))
			{
				int scanned = sscanf(line.data(), "jnz %s %s",
					paramA,
					paramB);
				assert(scanned == 2);
				(void)scanned;

				if (isdigit(paramA[0]))
				{
					inst.Op = Opcode::JnzImm;
					inst.A = atoi(paramA);
				}
				else
				{
					inst.Op = Opcode::Jnz;
					inst.A = Register(paramA[0]);
				}
				inst.B = atoi(paramB);
			}
			else
			{
				assert(false);
			}
		}
		program.push_back(inst);
	}

	return program;
}

static void Execute(const vector<Instruction>& program, Computer* computer)
{
	while (computer->PC < program.size())
	{
		switch (program[computer->PC].Op)
		{
		case Opcode::CpyImm:
			computer->Registers[program[computer->PC].B] = program[computer->PC].A;
			break;

		case Opcode::Cpy:
			computer->Registers[program[computer->PC].B] = computer->Registers[program[computer->PC].A];
			break;

		case Opcode::Inc:
			computer->Registers[program[computer->PC].A]++;
			break;

		case Opcode::Dec:
			computer->Registers[program[computer->PC].A]--;
			break;

		case Opcode::JnzImm:
			if (program[computer->PC].A != 0)
			{
				computer->PC += program[computer->PC].B - 1;
			}
			break;

		case Opcode::Jnz:
			if (computer->Registers[program[computer->PC].A] != 0)
			{
				computer->PC += program[computer->PC].B - 1;
			}
			break;

		default:
			assert(false);
			break;
		}

		computer->PC += 1;
	}
}

void Puzzle12_A_2016()
{
	vector<Instruction> program = ParseProgram();

	Computer comp;
	Execute(program, &comp);

	int32_t answer = comp.Registers[0];
	return PuzzleOutput::Submit(2016, 12, 1, answer);
}

void Puzzle12_B_2016()
{
	vector<Instruction> program = ParseProgram();

	Computer comp;
	comp.Registers[2] = 1;
	Execute(program, &comp);

	int32_t answer = comp.Registers[0];
	return PuzzleOutput::Submit(2016, 12, 2, answer);
}

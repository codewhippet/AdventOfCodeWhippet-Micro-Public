#include "stdafx.h"

using namespace std;

namespace Puzzle23_2015_Types
{
	enum class Opcode
	{
		Hlf,
		Tpl,
		Inc,
		Jmp,
		Jie,
		Jio
	};

	struct Instruction
	{
		Opcode Op;
		int32_t Reg;
		int32_t Immediate;
	};

	struct Computer
	{
		int32_t PC = 0;
		int32_t Registers[2] = { 0 };
	};
}

using namespace Puzzle23_2015_Types;

static vector<Instruction> ParseProgram()
{
	vector<Instruction> program;

	const map<string_view, Opcode> opcodes =
	{
		{ "hlf", Opcode::Hlf },
		{ "tpl", Opcode::Tpl },
		{ "inc", Opcode::Inc },
		{ "jmp", Opcode::Jmp },
		{ "jie", Opcode::Jie },
		{ "jio", Opcode::Jio }
	};

	char lineBuffer[32];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(lineBuffer, sizeof(lineBuffer));
		string_view line(lineBuffer);

		Instruction inst;
		{
			char instAsm[16] = { 0 };
			char reg = 'a';
			char immediate[16] = { 0 };

			if (line.find(',') != string::npos)
			{
				int scanned = sscanf(line.data(), "%s %c, %s",
					instAsm,
					&reg,
					immediate);
				assert(scanned == 3);
				(void)scanned;
			}
			else if (line.starts_with("jmp"))
			{
				int scanned = sscanf(line.data(), "%s %s",
					instAsm,
					immediate);
				assert(scanned == 2);
				(void)scanned;
			}
			else
			{
				int scanned = sscanf(line.data(), "%s %c",
					instAsm,
					&reg);
				assert(scanned == 2);
				(void)scanned;
			}

			inst.Op = opcodes.at(instAsm);
			inst.Reg = reg - 'a';
			inst.Immediate = atoi(immediate);
		}
		program.push_back(inst);
	}

	return program;
}

static void Execute(const vector<Instruction>& program, Computer* computer)
{
	while ((computer->PC >= 0) && (computer->PC < (int32_t)program.size()))
	{
		switch (program[computer->PC].Op)
		{
		case Opcode::Hlf:
			computer->Registers[program[computer->PC].Reg] /= 2;
			break;

		case Opcode::Tpl:
			computer->Registers[program[computer->PC].Reg] *= 3;
			break;

		case Opcode::Inc:
			computer->Registers[program[computer->PC].Reg] += 1;
			break;

		case Opcode::Jmp:
			computer->PC += program[computer->PC].Immediate - 1;
			break;

		case Opcode::Jie:
			if ((computer->Registers[program[computer->PC].Reg] % 2) == 0)
			{
				computer->PC += program[computer->PC].Immediate - 1;
			}
			break;

		case Opcode::Jio:
			if (computer->Registers[program[computer->PC].Reg] == 1)
			{
				computer->PC += program[computer->PC].Immediate - 1;
			}
			break;
		}

		computer->PC += 1;
	}
}

void Puzzle23_A_2015()
{
	vector<Instruction> program = ParseProgram();

	Computer comp;
	Execute(program, &comp);

	int32_t answer = comp.Registers[1];

	return PuzzleOutput::Submit(2015, 23, 1, answer);
}


void Puzzle23_B_2015()
{
	vector<Instruction> program = ParseProgram();

	Computer comp;
	comp.Registers[0] = 1;
	Execute(program, &comp);

	int32_t answer = comp.Registers[1];

	return PuzzleOutput::Submit(2015, 23, 2, answer);
}

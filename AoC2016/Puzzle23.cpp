#include "stdafx.h"

using namespace std;

namespace Puzzle23_2016_Types
{
	enum class Opcode : int32_t
	{
		Cpy,
		Inc,
		Dec,
		Jnz,
		Tgl,

		Nop,
		Add,
		Mul,

		Count
	};

	enum class ArgType : uint8_t
	{
		Invalid,
		Immediate,
		Register,
	};

	struct Instruction
	{
		Opcode Op;
		pair<ArgType, int32_t> A = { ArgType::Invalid, 0 };
		pair<ArgType, int32_t> B = { ArgType::Invalid, 0 };
	};

	struct Computer
	{
		int32_t PC = 0;
		int32_t Registers[4] = { 0 };
	};
}

using namespace Puzzle23_2016_Types;

static pair<ArgType, int32_t> ParseParam(const char* param)
{
	return isalpha(param[0]) ? pair<ArgType, int32_t>{ ArgType::Register, param[0] - 'a' } : pair<ArgType, int32_t>{ ArgType::Immediate, atoi(param) };
}

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

				inst.Op = Opcode::Cpy;
				inst.A = ParseParam(paramA);
				inst.B = ParseParam(paramB);
			}
			else if (line.starts_with("inc"sv))
			{
				int scanned = sscanf(line.data(), "inc %s",
					paramA);
				assert(scanned == 1);
				(void)scanned;

				inst.Op = Opcode::Inc;
				inst.A = ParseParam(paramA);
			}
			else if (line.starts_with("dec"sv))
			{
				int scanned = sscanf(line.data(), "dec %s",
					paramA);
				assert(scanned == 1);
				(void)scanned;

				inst.Op = Opcode::Dec;
				inst.A = ParseParam(paramA);
			}
			else if (line.starts_with("jnz"sv))
			{
				int scanned = sscanf(line.data(), "jnz %s %s",
					paramA,
					paramB);
				assert(scanned == 2);
				(void)scanned;

				inst.Op = Opcode::Jnz;
				inst.A = ParseParam(paramA);
				inst.B = ParseParam(paramB);

			}
			else if (line.starts_with("tgl"sv))
			{
				int scanned = sscanf(line.data(), "tgl %s",
					paramA);
				assert(scanned == 1);
				(void)scanned;

				inst.Op = Opcode::Tgl;
				inst.A = ParseParam(paramA);
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

static void Execute(vector<Instruction>* program, Computer* computer)
{
	auto ReadParam = [&](const pair<ArgType, int32_t>& param) -> int32_t
		{
			if (param.first == ArgType::Immediate)
			{
				return param.second;
			}

			if (param.first == ArgType::Register)
			{
				assert((param.second >= 0) && (param.second < 4));
				return computer->Registers[param.second];
			}

			assert(false);
			return 0;
		};

	const array<pair<Opcode, Opcode>, static_cast<size_t>(Opcode::Count)> toggleOp =
	{
		pair<Opcode, Opcode>{ Opcode::Cpy, Opcode::Jnz },
		pair<Opcode, Opcode>{ Opcode::Inc, Opcode::Dec },
		pair<Opcode, Opcode>{ Opcode::Dec, Opcode::Inc },
		pair<Opcode, Opcode>{ Opcode::Jnz, Opcode::Cpy },
		pair<Opcode, Opcode>{ Opcode::Tgl, Opcode::Inc },
		pair<Opcode, Opcode>{ Opcode::Nop, Opcode::Count },
		pair<Opcode, Opcode>{ Opcode::Add, Opcode::Count },
		pair<Opcode, Opcode>{ Opcode::Mul, Opcode::Count },
	};

	while (computer->PC < program->size())
	{
		const Instruction& currentInst = (*program)[computer->PC];
		switch (currentInst.Op)
		{
		case Opcode::Cpy:
			if (currentInst.B.first == ArgType::Register)
			{
				computer->Registers[currentInst.B.second] = ReadParam(currentInst.A);
			}
			break;

		case Opcode::Inc:
			if (currentInst.A.first == ArgType::Register)
			{
				computer->Registers[currentInst.A.second]++;
			}
			break;

		case Opcode::Dec:
			if (currentInst.A.first == ArgType::Register)
			{
				computer->Registers[currentInst.A.second]--;
			}
			break;

		case Opcode::Jnz:
			if (ReadParam(currentInst.A) != 0)
			{
				int32_t bVal = ReadParam(currentInst.B);
				computer->PC += bVal - 1;
			}
			break;

		case Opcode::Tgl:
			{
				int32_t target = computer->PC + ReadParam(currentInst.A);
				if (target >= 0 && target < (int32_t)program->size())
				{
					(*program)[target].Op = toggleOp[static_cast<size_t>((*program)[target].Op)].second;
				}
			}
			break;

		case Opcode::Nop:
			break;

		case Opcode::Add:
			if (currentInst.B.first == ArgType::Register)
			{
				computer->Registers[currentInst.B.second] += ReadParam(currentInst.A);
			}
			break;

		case Opcode::Mul:
			if (currentInst.B.first == ArgType::Register)
			{
				computer->Registers[currentInst.B.second] *= ReadParam(currentInst.A);
			}
			break;

		default:
			assert(false);
			break;
		}

		computer->PC += 1;
	}
}

static void PatchMultiplyIntoProgram(vector<Instruction>* program)
{
	// High level:
	// a += b * d
	// c = 0
	// d = 0

	// New asm:
	// cpy b c
	// mul d c
	// add c a
	// cpy 0 c
	// cpy 0 d

	(*program)[4] = { Opcode::Cpy, { ArgType::Register, 1 }, { ArgType::Register, 2 } };
	(*program)[5] = { Opcode::Mul, { ArgType::Register, 3 }, { ArgType::Register, 2 } };
	(*program)[6] = { Opcode::Add, { ArgType::Register, 2 }, { ArgType::Register, 0 } };
	(*program)[7] = { Opcode::Cpy, { ArgType::Immediate, 0 }, { ArgType::Register, 2 } };
	(*program)[8] = { Opcode::Cpy, { ArgType::Immediate, 0 }, { ArgType::Register, 3 } };
}

void Puzzle23_A_2016()
{
	vector<Instruction> program = ParseProgram();
	PatchMultiplyIntoProgram(&program);

	Computer comp;
	comp.Registers[0] = 7;
	Execute(&program, &comp);

	int32_t answer = comp.Registers[0];
	return PuzzleOutput::Submit(2016, 23, 1, answer);
}

void Puzzle23_B_2016()
{
	vector<Instruction> program = ParseProgram();
	PatchMultiplyIntoProgram(&program);

	Computer comp;
	comp.Registers[0] = 12;
	Execute(&program, &comp);

	int32_t answer = comp.Registers[0];
	return PuzzleOutput::Submit(2016, 23, 2, answer);
}

#include "stdafx.h"

using namespace std;

namespace Puzzle25_2016_Types
{
	enum class Opcode
	{
		Cpy,
		Inc,
		Dec,
		Jnz,
		Out,

		Add,
		Mul,
		And,
		Shr,
	};

	enum class ArgType : uint8_t
	{
		Invalid,
		Immediate,
		Register,
	};

	enum : int32_t
	{
		A = 0,
		B = 1,
		C = 2,
		D = 3,
	};

	struct Instruction
	{
		Opcode Op;
		pair<ArgType, int32_t> A = { ArgType::Invalid, 0 };
		pair<ArgType, int32_t> B = { ArgType::Invalid, 0 };
	};

	struct Computer
	{
		size_t PC = 0;
		int32_t Registers[4] = { 0 };

		auto operator<=>(const Computer& other) const = default;
	};
}

using namespace Puzzle25_2016_Types;

static pair<ArgType, int32_t> ParseParam(const char* param)
{
	return isalpha(param[0]) ? pair<ArgType, int32_t>{ ArgType::Register, param[0] - 'a' } : pair<ArgType, int32_t>{ ArgType::Immediate, atoi(param) };
}

static uvector<Instruction> ParseProgram()
{
	uvector<Instruction> program;
	program.reserve(32);

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
			else if (line.starts_with("out"sv))
			{
				int scanned = sscanf(line.data(), "out %s",
					paramA);
				assert(scanned == 1);
				(void)scanned;

				inst.Op = Opcode::Out;
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

static bool Execute(const uvector<Instruction>& program,
	Computer* computer,
	size_t instructionLimit)
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

	umap<Computer, size_t> previousStates;
	size_t outputCount = 0;

	size_t instructionCount = 0;
	while (computer->PC < program.size())
	{
		if (++instructionCount > instructionLimit)
		{
			return false;
		}

		// Infinite loop check
		if (previousStates.contains(*computer))
		{
			// If we're in an infinite loop, and we've output a non-zero even number of tokens since the start of the loop,
			// then we'll continue to output the correct tokens forever
			if ((outputCount > 0) && ((outputCount % 2) == 0))
			{
				return true;
			}

			return false;
		}

		previousStates.insert({ *computer, instructionCount });

		const Instruction& currentInst = program[computer->PC];
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

		case Opcode::Out:
			{
				int32_t expectedOutput = (outputCount % 2);
				if (ReadParam(currentInst.A) == expectedOutput)
				{
					outputCount++;
				}
				else
				{
					return false;
				}
			}
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

		case Opcode::And:
			if (currentInst.B.first == ArgType::Register)
			{
				computer->Registers[currentInst.B.second] &= ReadParam(currentInst.A);
			}
			break;

		case Opcode::Shr:
			if (currentInst.B.first == ArgType::Register)
			{
				computer->Registers[currentInst.B.second] >>= ReadParam(currentInst.A);
			}
			break;

		default:
			assert(false);
			break;
		}

		computer->PC += 1;
	}

	return false;
}

void PatchMultiplyIntoProgram(uvector<Instruction>* program)
{
	// Original asm:
	// cpy 182 b
	// inc d
	// dec b
	// jnz b -2
	// dec c
	// jnz c -5

	// High level:
	// d += 182 * c
	// b = 0
	// c = 0

	// New asm:
	// cpy 182 b
	// mul b c
	// add c d
	// cpy 0 b
	// cpy 0 c
	// jnz c -5

	size_t inst = 3;
	(*program)[inst++] = { Opcode::Mul, { ArgType::Register, B }, { ArgType::Register, C } };
	(*program)[inst++] = { Opcode::Add, { ArgType::Register, C }, { ArgType::Register, D } };
	(*program)[inst++] = { Opcode::Cpy, { ArgType::Immediate, 0 }, { ArgType::Register, B } };
	(*program)[inst++] = { Opcode::Cpy, { ArgType::Immediate, 0 }, { ArgType::Register, C } };
}

void PatchReduceIntoProgram(uvector<Instruction>* program)
{
	// Original asm:
	// cpy 2 c
	// jnz b 2
	// jnz 1 6
	// dec b
	// dec c
	// jnz c -4
	// inc a
	// jnz 1 -7

	// High level:
	// a := b / 2
	// c := (b & 1) ? 1 : 2;
	// b := 0

	// New asm:
	// cpy b a
	// shr 1 a
	// and 1 b
	// jnz b 3
	// cpy 2 c
	// jnz 1 2
	// cpy 1 c
	// cpy 0 b

	size_t inst = 12;
	(*program)[inst++] = { Opcode::Cpy, { ArgType::Register, B }, { ArgType::Register, A } };
	(*program)[inst++] = { Opcode::Shr, { ArgType::Immediate, 1 }, { ArgType::Register, A } };
	(*program)[inst++] = { Opcode::And, { ArgType::Immediate, 1 }, { ArgType::Register, B } };
	(*program)[inst++] = { Opcode::Jnz, { ArgType::Register, B }, { ArgType::Immediate, 3 } };
	(*program)[inst++] = { Opcode::Cpy, { ArgType::Immediate, 2 }, { ArgType::Register, C } };
	(*program)[inst++] = { Opcode::Jnz, { ArgType::Immediate, 1 }, { ArgType::Immediate, 2 } };
	(*program)[inst++] = { Opcode::Cpy, { ArgType::Immediate, 1 }, { ArgType::Register, C } };
	(*program)[inst++] = { Opcode::Cpy, { ArgType::Immediate, 0 }, { ArgType::Register, B } };
}

void Puzzle25_A_2016()
{
	MemArenaSmallBlockCount sbas[] =
	{
		{ 384, 96 }
	};

	MemArenaConfig cfg;
	cfg.SmallBlockCounts = sbas;
	cfg.NumSmallBlockCounts = sizeof(sbas) / sizeof(sbas[0]);
	cfg.LargeBlockRegionSize = 2 * 1024;

	MemArena_Configure(cfg);
	{
		uvector<Instruction> program = ParseProgram();
		PatchMultiplyIntoProgram(&program);
		PatchReduceIntoProgram(&program);

		int32_t answer = 0;
		for (int32_t i = 0; i < 100000; i++)
		{
			Computer comp;
			comp.Registers[0] = i;
			if (Execute(program, &comp, 1000000))
			{
				answer = (int32_t)i;
				break;
			}
		}

		PuzzleOutput::Submit(2016, 25, 1, answer);
	}
	MemArena_Reset();
}

void Puzzle25_B_2016()
{
	return PuzzleOutput::Submit(2016, 25, 2, int32_t(-1));
}

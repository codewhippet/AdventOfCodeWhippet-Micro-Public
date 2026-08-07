#include "stdafx.h"

using namespace std;

namespace Puzzle16_2018_Types
{
	using Registers = array<int32_t, 4>;
	using Instruction = array<int8_t, 4>;

	struct Sample
	{
		Registers Before;
		Instruction Inst;
		Registers After;
	};

	enum RegisterNames : size_t
	{
		A = 1,
		B = 2,
		C = 3,
	};
}

using namespace Puzzle16_2018_Types;

static Registers addr(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] + reg[inst[B]];
    return reg;
}
static Registers addi(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] + inst[B];
    return reg;
}

static Registers mulr(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] * reg[inst[B]];
    return reg;
}
static Registers muli(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] * inst[B];
    return reg;
}

static Registers banr(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] & reg[inst[B]];
    return reg;
}
static Registers bani(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] & inst[B];
    return reg;
}

static Registers borr(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] | reg[inst[B]];
    return reg;
}
static Registers bori(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] | inst[B];
    return reg;
}

static Registers setr(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]];
    return reg;
}
static Registers seti(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = inst[A];
    return reg;
}

static Registers gtir(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = inst[A] > reg[inst[B]];
    return reg;
}
static Registers gtri(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] > inst[B];
    return reg;
}
static Registers gtrr(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] > reg[inst[B]];
    return reg;
}

static Registers eqir(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = inst[A] == reg[inst[B]];
    return reg;
}
static Registers eqri(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] == inst[B];
    return reg;
}
static Registers eqrr(const Instruction& inst, Registers reg)
{
    reg[inst[C]] = reg[inst[A]] == reg[inst[B]];
    return reg;
}

void Puzzle16_A_2018()
{
	const vector<Registers(*)(const Instruction&, Registers)> instructions =
	{
		addr, addi,
		mulr, muli,
		banr, bani,
		borr, bori,
		setr, seti,
		gtir, gtri, gtrr,
		eqir, eqri, eqrr,
	};

	int32_t answer = 0;

	while (PuzzleInput::PeekChar() == 'B')
	{
		Sample sample;
		ranges::for_each(sample.Before, [](int32_t& s) { s = Parse::GetInt32(); });
		PuzzleInput::DropLine();
		PuzzleInput::NextLine();

		ranges::for_each(sample.Inst, [](int8_t& s) { s = Parse::GetInt8(); });
		PuzzleInput::NextLine();

		ranges::for_each(sample.After, [](int32_t& s) { s = Parse::GetInt32(); });
		PuzzleInput::DropLine();
		PuzzleInput::NextLine();

		size_t matches = 0;
		for (const auto& inst : instructions)
		{
			if (inst(sample.Inst, sample.Before) == sample.After)
			{
				matches++;
			}
		}
		if (matches >= 3)
		{
			answer++;
		}
	}

	PuzzleOutput::Submit(2018, 16, 1, answer);
}

void Puzzle16_B_2018()
{
	const vector<Registers(*)(const Instruction&, Registers)> instructions =
	{
		addr, addi,
		mulr, muli,
		banr, bani,
		borr, bori,
		setr, seti,
		gtir, gtri, gtrr,
		eqir, eqri, eqrr,
	};

	vector<uint32_t> possibleOpcodes(instructions.size(), (1 << instructions.size()) - 1);
	while (PuzzleInput::PeekChar() == 'B')
	{
		Sample sample;
		ranges::for_each(sample.Before, [](int32_t& s) { s = Parse::GetInt32(); });
		PuzzleInput::DropLine();
		PuzzleInput::NextLine();

		ranges::for_each(sample.Inst, [](int8_t& s) { s = Parse::GetInt8(); });
		PuzzleInput::NextLine();

		ranges::for_each(sample.After, [](int32_t& s) { s = Parse::GetInt32(); });
		PuzzleInput::DropLine();
		PuzzleInput::NextLine();

		for (size_t i = 0; i < instructions.size(); i++)
		{
			const auto& inst = instructions[i];
			if (inst(sample.Inst, sample.Before) != sample.After)
			{
				possibleOpcodes[i] &= ~(1 << sample.Inst[0]);
			}
		}
	}

	vector<size_t> opcodeToInstruction(instructions.size(), numeric_limits<size_t>::max());
	for (size_t i = 0; i < opcodeToInstruction.size(); i++)
	{
		const auto uniqueOption = ranges::find_if(possibleOpcodes, [](const auto& s) { return popcount(s) == 1; });
		assert(uniqueOption != possibleOpcodes.end());

		uint32_t opcodeBit = *uniqueOption;
		uint32_t opcode = countr_zero(opcodeBit);
		size_t instruction = uniqueOption - possibleOpcodes.begin();

		opcodeToInstruction[opcode] = instruction;

		for (auto& possibleOpcode : possibleOpcodes)
		{
			possibleOpcode &= ~opcodeBit;
		}
	}

	Registers reg{ 0, 0, 0, 0 };
	while (PuzzleInput::NextLine())
	{
		Instruction inst;
		ranges::for_each(inst, [](int8_t& s) { s = Parse::GetInt8(); });

		int8_t opcode = inst[0];
		size_t instructionIndex = opcodeToInstruction[opcode];
		reg = instructions[instructionIndex](inst, reg);
	}

	int32_t answer = reg[0];

	PuzzleOutput::Submit(2018, 16, 2, answer);
}

#include "stdafx.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle19_2018_Types
{
	enum class Opcode
	{
		addr,
		addi,
		mulr,
		muli,
		banr,
		bani,
		borr,
		bori,
		setr,
		seti,
		gtir,
		gtri,
		gtrr,
		eqir,
		eqri,
		eqrr,
	};

	struct Instruction
	{
		Opcode Op;
		array<int32_t, 3> Args = { 0, 0, 0 };
	};

	struct Computer
	{
		int32_t& PC()
		{
			return Registers[PCReg];
		}

		int32_t PCReg = 0;
		array<int32_t, 6> Registers = { 0, 0, 0, 0, 0, 0 };
	};
}

using namespace Puzzle19_2018_Types;

static pair<vector<Instruction>, int32_t> ParseProgram()
{
	vector<Instruction> program;
	program.reserve(48);

	const vector<pair<string_view, Opcode>> assemblyTable =
	{
		{ "addr"sv, Opcode::addr },
		{ "addi"sv, Opcode::addi },
		{ "mulr"sv, Opcode::mulr },
		{ "muli"sv, Opcode::muli },
		{ "banr"sv, Opcode::banr },
		{ "bani"sv, Opcode::bani },
		{ "borr"sv, Opcode::borr },
		{ "bori"sv, Opcode::bori },
		{ "setr"sv, Opcode::setr },
		{ "seti"sv, Opcode::seti },
		{ "gtir"sv, Opcode::gtir },
		{ "gtri"sv, Opcode::gtri },
		{ "gtrr"sv, Opcode::gtrr },
		{ "eqir"sv, Opcode::eqir },
		{ "eqri"sv, Opcode::eqri },
		{ "eqrr"sv, Opcode::eqrr },
	};

	int32_t pcRegister = 0;
	while (PuzzleInput::NextLine())
	{
		if (PuzzleInput::PeekChar() == '#')
		{
			pcRegister = Parse::GetInt32();
		}
		else
		{
			Instruction inst;

			char instOp[5] = {};
			instOp[0] = static_cast<char>(PuzzleInput::GetChar());
			instOp[1] = static_cast<char>(PuzzleInput::GetChar());
			instOp[2] = static_cast<char>(PuzzleInput::GetChar());
			instOp[3] = static_cast<char>(PuzzleInput::GetChar());

			inst.Args[0] = Parse::GetInt32();
			inst.Args[1] = Parse::GetInt32();
			inst.Args[2] = Parse::GetInt32();

			for (const auto& asmInst : assemblyTable)
			{
				if (instOp == asmInst.first)
				{
					inst.Op = asmInst.second;
					break;
				}
			}

			program.push_back(inst);
		}
	}

	return { program, pcRegister };
}

static void Execute(const vector<Instruction>& program, Computer* computer, int32_t runFor)
{
	enum : size_t
	{
		A = 0,
		B = 1,
		C = 2,
	};

	for (int32_t i = 0; i < runFor; i++)
	{
		const Instruction& currentInst = program[computer->PC()];
		switch (currentInst.Op)
		{
		[[likely]] case Opcode::addr:
			computer->Registers[currentInst.Args[C]] = computer->Registers[currentInst.Args[A]] + computer->Registers[currentInst.Args[B]];
			break;
		[[likely]] case Opcode::addi:
			computer->Registers[currentInst.Args[C]] = computer->Registers[currentInst.Args[A]] + currentInst.Args[B];
			break;
		case Opcode::mulr:
			computer->Registers[currentInst.Args[C]] = computer->Registers[currentInst.Args[A]] * computer->Registers[currentInst.Args[B]];
			break;
		case Opcode::eqrr:
			computer->Registers[currentInst.Args[C]] = computer->Registers[currentInst.Args[A]] == computer->Registers[currentInst.Args[B]];
			break;
		case Opcode::seti:
			computer->Registers[currentInst.Args[C]] = currentInst.Args[A];
			break;
		case Opcode::setr:
			computer->Registers[currentInst.Args[C]] = computer->Registers[currentInst.Args[A]];
			break;
		case Opcode::gtrr:
			computer->Registers[currentInst.Args[C]] = computer->Registers[currentInst.Args[A]] > computer->Registers[currentInst.Args[B]];
			break;
		[[unlikely]] case Opcode::muli:
			computer->Registers[currentInst.Args[C]] = computer->Registers[currentInst.Args[A]] * currentInst.Args[B];
			break;
		[[unlikely]] default:
			assert(false);
		}

		int32_t nextPc = computer->PC() + 1;
		if (nextPc >= (int32_t)program.size())
		{
			break;
		}
		computer->PC() = nextPc;
	}
}

void Puzzle19_A_2018()
{
	auto [program, pcRegister] = ParseProgram();

	Computer comp;
	comp.PCReg = pcRegister;
	Execute(program, &comp, numeric_limits<int32_t>::max());

	int32_t answer = comp.Registers[0];
	PuzzleOutput::Submit(2018, 19, 1, answer);
}

void Puzzle19_B_2018()
{
	auto [program, pcRegister] = ParseProgram();

	Computer comp;
	comp.PCReg = pcRegister;
	comp.Registers[0] = 1;
	Execute(program, &comp, (int32_t)program.size());

	int32_t target = comp.Registers[5];

	int32_t answer = 0;
	for (int32_t i = 1; (i * i) <= target; i++)
	{
		int32_t rem;
		int32_t div = Hardware::DivModRem(target, i, &rem);

		if (rem == 0)
		{
			answer += i;
			answer += div;
		}
	}

	PuzzleOutput::Submit(2018, 19, 2, answer);
}

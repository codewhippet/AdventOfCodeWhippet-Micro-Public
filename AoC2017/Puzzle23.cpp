#include "stdafx.h"

using namespace std;

namespace Puzzle23_2017_Types
{
	enum class Opcode
	{
		Set,
		Sub,
		Mul,
		Jnz,
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
		vector<int32_t> Registers = vector<int32_t>(256, 0);
		int32_t MulCount = 0;
	};
}

using namespace Puzzle23_2017_Types;

static pair<ArgType, int32_t> ParseParam(const char* param)
{
	return isalpha(param[0]) ? pair<ArgType, int32_t>{ ArgType::Register, param[0] } : pair<ArgType, int32_t>{ ArgType::Immediate, atoi(param) };
}

static Instruction ParseDoubleArgInstruction(Opcode op, const char* format, const string_view& line)
{
	char paramA[16] = { 0 };
	char paramB[16] = { 0 };

	int scanned = sscanf(line.data(), format, paramA, paramB);
	assert(scanned == 2);
	(void)scanned;

	return { op, ParseParam(paramA), ParseParam(paramB) };
}

static vector<Instruction> ParseProgram()
{
	vector<Instruction> program;

	const vector<pair<string_view, function<Instruction(const string_view&)>>> assemblyTable =
	{
		{ "set"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Set, "set %s %s", line); } },
		{ "sub"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Sub, "sub %s %s", line); } },
		{ "mul"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Mul, "mul %s %s", line); } },
		{ "jnz"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Jnz, "jnz %s %s", line); } },
	};

	char lineBuffer[16];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(lineBuffer);
		string_view line{ lineBuffer };

		for (const auto& asmInst : assemblyTable)
		{
			if (line.starts_with(asmInst.first))
			{
				program.push_back(asmInst.second(line));
				break;
			}
		}
	}

	return program;
}

static void Execute(const vector<Instruction>& program, Computer* computer)
{
	auto ReadParam = [&](const pair<ArgType, int32_t>& param) -> int32_t
		{
			if (param.first == ArgType::Immediate)
			{
				return param.second;
			}

			if (param.first == ArgType::Register)
			{
				return computer->Registers[(char)param.second];
			}

			assert(false);
			return 0;
		};

	while ((computer->PC >= 0) && (computer->PC < program.size()))
	{
		const Instruction& currentInst = program[computer->PC];
		switch (currentInst.Op)
		{
		case Opcode::Set:
			assert(currentInst.A.first == ArgType::Register);
			computer->Registers[(char)currentInst.A.second] = ReadParam(currentInst.B);
			break;
		case Opcode::Sub:
			assert(currentInst.A.first == ArgType::Register);
			computer->Registers[(char)currentInst.A.second] -= ReadParam(currentInst.B);
			break;
		case Opcode::Mul:
			assert(currentInst.A.first == ArgType::Register);
			computer->Registers[(char)currentInst.A.second] *= ReadParam(currentInst.B);
			computer->MulCount++;
			break;
		case Opcode::Jnz:
			if (ReadParam(currentInst.A) != 0)
			{
				computer->PC += static_cast<int32_t>(ReadParam(currentInst.B) - 1);
			}
			break;
		}
		computer->PC += 1;
	}
}

static int32_t SearchForCompositeNumbers(int32_t from, int32_t to, int32_t increment)
{
	vector<int8_t> sieve(to - from + 1);
	int32_t endFactor = static_cast<int32_t>(ceil(sqrtf(static_cast<float>(to)))) + 1;
	for (int32_t factor = 2; factor < endFactor; factor++)
	{
		int32_t firstMultipleToMark = (((from + factor - 1) / factor) * factor); // First multiple in the window
		int32_t firstMultipleWorthMarking = factor * factor; // Make sure we don't mark prime factors as composite
		if (firstMultipleWorthMarking > firstMultipleToMark)
		{
			firstMultipleToMark = firstMultipleWorthMarking;
		}

		int32_t chunkIndex = firstMultipleToMark - from;
		for (/***/; chunkIndex < sieve.size(); chunkIndex += factor)
		{
			sieve[chunkIndex] = 1;
		}
	}

	int32_t composite = 0;
	for (size_t i = 0; i < sieve.size(); i += increment)
	{
		composite += sieve[i];
	}
	return composite;
}

void Puzzle23_A_2017()
{
	vector<Instruction> program = ParseProgram();
	Computer comp;
	Execute(program, &comp);

	int32_t answer = comp.MulCount;

	PuzzleOutput::Submit(2017, 23, 1, answer);
}


void Puzzle23_B_2017()
{
	vector<Instruction> program = ParseProgram();

	auto ReadConstant = [](const pair<ArgType, int32_t>& param) -> int32_t
		{
			if (param.first == ArgType::Immediate)
			{
				return param.second;
			}

			assert(false);
			return 0;
		};

	int32_t from = ReadConstant(program[0].B);
	from *= ReadConstant(program[4].B);
	from -= ReadConstant(program[5].B);

	int32_t to = from;
	to -= ReadConstant(program[7].B);

	int32_t increment = -ReadConstant(program[30].B);

	int32_t answer = SearchForCompositeNumbers(from, to, increment);

	PuzzleOutput::Submit(2017, 23, 2, answer);
}

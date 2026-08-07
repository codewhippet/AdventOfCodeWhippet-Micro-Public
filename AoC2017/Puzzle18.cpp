#include "stdafx.h"

using namespace std;

namespace Puzzle18_2017_Types
{
	enum class Opcode
	{
		Snd,
		Set,
		Add,
		Mul,
		Mod,
		Rcv,
		Jgz,
	};

	enum class ArgType : uint8_t
	{
		Invalid,
		Immediate,
		Register,
	};

	enum class HaltType : uint8_t
	{
		Finished,
		Waiting,
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
		vector<int64_t> Registers = vector<int64_t>(256, 0);
		deque<int32_t> RecvQueue;
		deque<int32_t>* SendTo;
		int32_t SendCount = 0;
	};
}

using namespace Puzzle18_2017_Types;

static pair<ArgType, int32_t> ParseParam(const char* param)
{
	return isalpha(param[0]) ? pair<ArgType, int32_t>{ ArgType::Register, param[0] } : pair<ArgType, int32_t>{ ArgType::Immediate, atoi(param) };
}

static Instruction ParseSingleArgInstruction(Opcode op, const char *format, const string_view& line)
{
	char paramA[16] = { 0 };

	int scanned = sscanf(line.data(), format, paramA);
	assert(scanned == 1);
	(void)scanned;

	return { op, ParseParam(paramA) };
}

static Instruction ParseDoubleArgInstruction(Opcode op, const char *format, const string_view& line)
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
		{ "snd"sv, [](const string_view& line) { return ParseSingleArgInstruction(Opcode::Snd, "snd %s", line); } },
		{ "set"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Set, "set %s %s", line); } },
		{ "add"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Add, "add %s %s", line); } },
		{ "mul"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Mul, "mul %s %s", line); } },
		{ "mod"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Mod, "mod %s %s", line); } },
		{ "rcv"sv, [](const string_view& line) { return ParseSingleArgInstruction(Opcode::Rcv, "rcv %s", line); } },
		{ "jgz"sv, [](const string_view& line) { return ParseDoubleArgInstruction(Opcode::Jgz, "jgz %s %s", line); } },
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

static HaltType Execute(const vector<Instruction>& program, Computer* computer)
{
	auto ReadParam = [&](const pair<ArgType, int32_t>& param) -> int64_t
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
		case Opcode::Snd:
			computer->SendTo->push_back(static_cast<int32_t>(ReadParam(currentInst.A)));
			computer->SendCount++;
			break;
		case Opcode::Set:
			assert(currentInst.A.first == ArgType::Register);
			computer->Registers[(char)currentInst.A.second] = ReadParam(currentInst.B);
			break;
		case Opcode::Add:
			assert(currentInst.A.first == ArgType::Register);
			computer->Registers[(char)currentInst.A.second] += ReadParam(currentInst.B);
			break;
		case Opcode::Mul:
			assert(currentInst.A.first == ArgType::Register);
			computer->Registers[(char)currentInst.A.second] *= ReadParam(currentInst.B);
			break;
		case Opcode::Mod:
			assert(currentInst.A.first == ArgType::Register);
			computer->Registers[(char)currentInst.A.second] %= ReadParam(currentInst.B);
			break;
		case Opcode::Rcv:
			assert(currentInst.A.first == ArgType::Register);
			if (computer->RecvQueue.empty())
			{
				return HaltType::Waiting;
			}
			computer->Registers[(char)currentInst.A.second] = computer->RecvQueue.front();
			computer->RecvQueue.pop_front();
			break;
		case Opcode::Jgz:
			if (ReadParam(currentInst.A) > 0)
			{
				computer->PC += static_cast<int32_t>(ReadParam(currentInst.B) - 1);
			}
			break;
		}
		computer->PC += 1;
	}

	return HaltType::Finished;
}

void Puzzle18_A_2017()
{
	vector<Instruction> program = ParseProgram();
	deque<int32_t> output;
	Computer comp;
	comp.SendTo = &output;
	Execute(program, &comp);

	int32_t answer = static_cast<int32_t>(output.back());

	PuzzleOutput::Submit(2017, 18, 1, answer);
}

void Puzzle18_B_2017()
{
	vector<Instruction> program = ParseProgram();

	vector<Computer> computers(2);
	computers[0].SendTo = &computers[1].RecvQueue;
	computers[0].Registers['p'] = 0;

	computers[1].SendTo = &computers[0].RecvQueue;
	computers[1].Registers['p'] = 1;

	while (true)
	{
		int32_t oldSendCounts[2] = { computers[0].SendCount, computers[1].SendCount };
		HaltType haltTypes[2];

		haltTypes[0] = Execute(program, &computers[0]);
		haltTypes[1] = Execute(program, &computers[1]);

		// Both terminated
		if ((haltTypes[0] == HaltType::Finished) && (haltTypes[1] == HaltType::Finished))
		{
			break;
		}

		// Both waiting and neither sending
		if ((haltTypes[0] == HaltType::Waiting) && (computers[1].SendCount == oldSendCounts[1]) &&
			(haltTypes[1] == HaltType::Waiting) && (computers[0].SendCount == oldSendCounts[0]))
		{
			break;
		}
	}

	int32_t answer = computers[1].SendCount;

	PuzzleOutput::Submit(2017, 18, 1, answer);
}

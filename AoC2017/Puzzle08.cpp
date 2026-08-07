#include "stdafx.h"

using namespace std;

namespace Puzzle08_2017_Types
{
	struct Instruction
	{
		size_t DestinationRegister;
		int32_t Amount;

		size_t TestRegister;
		function<bool(int32_t)> TestCondition;
	};

	struct Program
	{
		vector<Instruction> Instructions;
		size_t RegisterCount = 0;
	};
}

using namespace Puzzle08_2017_Types;

static Program ParseProgram()
{
	Program ret;

	const map<string_view, function<bool(int32_t, int32_t)>> tests =
	{
		{ string_view{ "==" }, [](int32_t amount, int32_t compareTo) { return amount == compareTo; } },
		{ string_view{ "!=" }, [](int32_t amount, int32_t compareTo) { return amount != compareTo; } },
		{ string_view{ ">" }, [](int32_t amount, int32_t compareTo) { return amount > compareTo; } },
		{ string_view{ "<" }, [](int32_t amount, int32_t compareTo) { return amount < compareTo; } },
		{ string_view{ ">=" }, [](int32_t amount, int32_t compareTo) { return amount >= compareTo; } },
		{ string_view{ "<=" }, [](int32_t amount, int32_t compareTo) { return amount <= compareTo; } },
	};

	ret.Instructions.reserve(1000);

	HashMap<string, size_t> registerIds(64, {});
	auto getRegisterId = [&](const char* name)
		{
			string nameStr{ name };

			size_t id;
			if (registerIds.TryFind(nameStr, &id) == false)
			{
				id = registerIds.Size();
				registerIds.Insert(nameStr, id);
			}
			return id;
		};

	char line[64];
	while (PuzzleInput::NextLine())
	{
		Parse::ReadNonEmptyLine(line);

		char destReg[4] = {};
		char op[4] = {};
		int32_t amount = 0;
		char testReg[4] = {};
		char condition[4] = {};
		int32_t testAmount = 0;
		int scanned = sscanf(line, "%s %s %d if %s %s %d", destReg, op, &amount, testReg, condition, &testAmount);
		assert(scanned == 6);
		(void)scanned;

		Instruction inst;
		inst.DestinationRegister = getRegisterId(destReg);
		inst.Amount = amount;

		inst.TestRegister = getRegisterId(testReg);
		inst.TestCondition = [test = tests.at(condition), amount = testAmount](int32_t regValue) -> bool { return test(regValue, amount); };

		if (strcmp(op, "dec") == 0)
		{
			inst.Amount = -inst.Amount;
		}

		ret.Instructions.push_back(inst);
	}

	ret.RegisterCount = registerIds.Size();
	return ret;
}

void Puzzle08_A_2017()
{
	Program program = ParseProgram();
	vector<int32_t> registers(program.RegisterCount);

	for (const Instruction& inst : program.Instructions)
	{
		if (inst.TestCondition(registers[inst.TestRegister]))
		{
			registers[inst.DestinationRegister] += inst.Amount;
		}
	}

	int32_t answer = ranges::max(registers);
	PuzzleOutput::Submit(2017, 8, 1, answer);
}

void Puzzle08_B_2017()
{
	Program program = ParseProgram();
	vector<int32_t> registers(program.RegisterCount);

	int32_t answer = numeric_limits<int32_t>::min();
	for (const Instruction& inst : program.Instructions)
	{
		if (inst.TestCondition(registers[inst.TestRegister]))
		{
			registers[inst.DestinationRegister] += inst.Amount;
		}
		answer = max(answer, ranges::max(registers));
	}

	PuzzleOutput::Submit(2017, 8, 2, answer);
}

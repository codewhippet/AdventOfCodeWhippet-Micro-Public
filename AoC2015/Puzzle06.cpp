#include "stdafx.h"

using namespace std;

namespace Puzzle06_2015_Types
{
	enum class Operation : uint8_t
	{
		TurnOn,
		TurnOff,
		Toggle,
	};

	struct Instruction
	{
		Operation Op;
		Vec2Int Begin;
		Vec2Int End;
	};
}

using namespace Puzzle06_2015_Types;

static Operation ParseOperation(const char* line)
{
	if (line[1] == 'o')
	{
		return Operation::Toggle;
	}
	return (line[6] == 'n') ? Operation::TurnOn : Operation::TurnOff;
}

// Grid update logic derived from a Karnaugh Map:
//
//      toggle  0  0  1  1
//     turnOff  0  1  0  1
// in | turnOn
//  0     0     x  0  1  x
//  0     1     1  x  x  x
//  1     0     x  0  0  x
//  1     1     1  x  x  x
//
// -->
// 
//      toggle  0  0  1  1
//     turnOff  0  1  0  1
// in | turnOn
//  0     0     A  0  B  B
//  0     1     A  0  B  B
//  1     0     A  0  0  0
//  1     1     A  0  0  0
//
// A = !toggle & !turnOff
// B = toggle & !in

static void UpdateRowOnOff(
	uint8_t turnOn,
	uint8_t toggle,
	uint8_t turnOff,
	int32_t begin,
	int32_t end,
	vector<uint8_t>* row)
{
	(void)turnOn;

	for (int32_t x = begin; x < end; x++)
	{
		uint8_t in = (*row)[x];
		(*row)[x] = (~toggle & ~turnOff) | (toggle & ~in);
	}
}

static void UpdateRowBrightness(
	int32_t change,
	int32_t begin,
	int32_t end,
	vector<int32_t>* row)
{
	for (int32_t x = begin; x < end; x++)
	{
		int32_t in = (*row)[x];
		(*row)[x] = max<int32_t>(in + change, 0);
	}
}

void Puzzle06_A_2015()
{
	const int32_t gridWidth = 1000;
	const int32_t gridHeight = 1000;

	vector<Instruction> instructions;
	instructions.reserve(512);
	while (PuzzleInput::NextLine())
	{
		char line[64];
		int32_t read = Parse::ReadNonDigits(line, sizeof(line));
		assert(read > 0);
		(void)read;

		instructions.push_back({
			ParseOperation(line),
			Vec2Int{ Parse::GetInt32(), Parse::GetInt32() },
			Vec2Int{ Parse::GetInt32() + 1, Parse::GetInt32() + 1 }
			});
	}

	int32_t answer = 0;

	vector<uint8_t> lights(gridWidth);
	for (int32_t y = 0; y < gridHeight; y++)
	{
		memset(lights.data(), 0x00, lights.size());
		for (const Instruction& inst : instructions)
		{
			if ((y >= inst.Begin.Y) && (y < inst.End.Y))
			{
				const uint8_t turnOn = (inst.Op == Operation::TurnOn ? 0xff : 0x00);
				const uint8_t toggle = (inst.Op == Operation::Toggle ? 0xff : 0x00);
				const uint8_t turnOff = (inst.Op == Operation::TurnOff ? 0xff : 0x00);

				UpdateRowOnOff(turnOn, toggle, turnOff, inst.Begin.X, inst.End.X, &lights);
			}
		}

		answer += (int32_t)ranges::count(lights, 0xff);
	}

	return PuzzleOutput::Submit(2015, 6, 1, answer);
}

void Puzzle06_B_2015()
{
	const int32_t gridWidth = 1000;
	const int32_t gridHeight = 1000;

	vector<Instruction> instructions;
	instructions.reserve(512);
	while (PuzzleInput::NextLine())
	{
		char line[64];
		int32_t read = Parse::ReadNonDigits(line, sizeof(line));
		assert(read > 0);
		(void)read;

		instructions.push_back({
			ParseOperation(line),
			Vec2Int{ Parse::GetInt32(), Parse::GetInt32() },
			Vec2Int{ Parse::GetInt32() + 1, Parse::GetInt32() + 1 }
			});
	}

	int32_t answer = 0;

	const array<int32_t, 3> opValues = { 1, -1, 2 };

	vector<int32_t> lights(gridWidth);
	for (int32_t y = 0; y < gridHeight; y++)
	{
		memset(lights.data(), 0x00, lights.size() * sizeof(decltype(lights)::value_type));
		for (const Instruction& inst : instructions)
		{
			if ((y >= inst.Begin.Y) && (y < inst.End.Y))
			{
				int32_t brightnessChange = opValues[(size_t)inst.Op];
				UpdateRowBrightness(brightnessChange, inst.Begin.X, inst.End.X, &lights);
			}
		}

		answer += accumulate(lights.begin(), lights.end(), 0, plus{});
	}

	return PuzzleOutput::Submit(2015, 6, 2, answer);
}

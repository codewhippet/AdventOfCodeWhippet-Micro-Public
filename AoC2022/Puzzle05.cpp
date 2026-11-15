#include "stdafx.h"

using namespace std;

namespace Puzzle05_2022_Types
{
}

using namespace Puzzle05_2022_Types;

static vector<string> ReadStacks()
{
	const size_t numStacks = 9;
	const size_t stackSpacing = 4;

	vector<string> stacks(numStacks);

	array<char, 48> line;
	while (PuzzleInput::NextLine())
	{
		int32_t len = Parse::ReadLine(line.data(), line.size());
		if (len < 2 || line[1] == '1')
			break;

		for (size_t i = 0; i < numStacks; i++)
		{
			stacks[i] += line[(i * stackSpacing) + 1];
		}
	}

	ranges::for_each(stacks,
		[](string& stack)
		{
			ranges::reverse(stack);
			stack.erase(ranges::find(stack, ' '), stack.end());
		});

	return stacks;
}

void Puzzle05_A_2022()
{
	vector<string> stacks = ReadStacks();

	while (PuzzleInput::NextLine())
	{
		int32_t moveN = Parse::GetInt32();
		int32_t from = Parse::GetInt32() - 1;
		int32_t to = Parse::GetInt32() - 1;

		for (int32_t i = 0; i < moveN; i++)
		{
			char c = stacks[from].back();
			stacks[from].resize(stacks[from].size() - 1);
			stacks[to] += c;
		}
	}

	string answer;
	ranges::copy(stacks | views::transform([](const auto &stack) { return stack.back(); }), back_inserter(answer));

	PuzzleOutput::Submit(2022, 5, 1, answer.c_str());
}

void Puzzle05_B_2022()
{
	vector<string> stacks = ReadStacks();

	while (PuzzleInput::NextLine())
	{
		int32_t moveN = Parse::GetInt32();
		int32_t from = Parse::GetInt32() - 1;
		int32_t to = Parse::GetInt32() - 1;

		size_t fromSize = stacks[from].size();
		stacks[to] += &stacks[from][fromSize - moveN];
		stacks[from].resize(fromSize - moveN);
	}

	string answer;
	ranges::copy(stacks | views::transform([](const auto &stack) { return stack.back(); }), back_inserter(answer));

	PuzzleOutput::Submit(2022, 5, 2, answer.c_str());
}

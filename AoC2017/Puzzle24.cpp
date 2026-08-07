#include "stdafx.h"

using namespace std;

namespace Puzzle24_2017_Types
{
	struct Component
	{
		int32_t From;
		int32_t To;
		array<uint32_t, 2> Id = {};
	};

	struct ExecutionContext
	{
		array<uint32_t, 2> CurrentBridge;
		int32_t CurrentStrength;
		int32_t StartingPins;
		size_t NextComponentToTry;
	};
}

using namespace Puzzle24_2017_Types;

static vector<vector<Component>> ParseComponents()
{
	const size_t highestPinCount = 64;
	vector<vector<Component>> ports(highestPinCount);
	ranges::for_each(ports, [](auto& vec) { vec.reserve(32); });

	int32_t lineNumber = 0;
	while (PuzzleInput::NextLine())
	{
		assert(lineNumber < 63);

		Component compFwd;
		compFwd.From = Parse::GetInt32();
		compFwd.To = Parse::GetInt32();
		PuzzleInput::DropLine();

		if (lineNumber < 32)
		{
			compFwd.Id[0] = 1 << lineNumber;
		}
		else
		{
			compFwd.Id[1] = 1 << (lineNumber - 32);
		}
		lineNumber++;

		ports[compFwd.From].push_back(compFwd);
		if (compFwd.To != compFwd.From)
		{
			swap(compFwd.From, compFwd.To);
			ports[compFwd.From].push_back(compFwd);
		}
	}

	return ports;
}

static int32_t StrongestBridge(const vector<vector<Component>>& components)
{
	vector<ExecutionContext> executionStack;
	executionStack.reserve(64);

	executionStack.push_back({ {}, 0, 0, 0 });

	int32_t strongest = 0;
	while (!executionStack.empty())
	{
		ExecutionContext& top = executionStack.back();
		const vector<Component>& nextComponents = components[top.StartingPins];
		if (top.NextComponentToTry == nextComponents.size())
		{
			executionStack.pop_back();
			continue;
		}

		const Component& nextComponent = components[top.StartingPins][top.NextComponentToTry];
		top.NextComponentToTry++;

		if (((top.CurrentBridge[0] & nextComponent.Id[0]) + (top.CurrentBridge[1] & nextComponent.Id[1])) == 0)
		{
			ExecutionContext newContext = top;
			newContext.CurrentBridge[0] |= nextComponent.Id[0];
			newContext.CurrentBridge[1] |= nextComponent.Id[1];
			newContext.CurrentStrength += nextComponent.From + nextComponent.To;
			newContext.StartingPins = nextComponent.To;
			newContext.NextComponentToTry = 0;
			executionStack.push_back(newContext);

			if (newContext.CurrentStrength > strongest)
			{
				strongest = newContext.CurrentStrength;
			}
		}
	}

	return strongest;
}

static int32_t LongestBridge(const vector<vector<Component>>& components)
{
	vector<ExecutionContext> executionStack;
	executionStack.reserve(64);

	executionStack.push_back({ {}, 0, 0, 0 });

	int32_t maxLength = 0;
	int32_t strengthOfMaxLength = 0;
	while (!executionStack.empty())
	{
		ExecutionContext& top = executionStack.back();
		const vector<Component>& nextComponents = components[top.StartingPins];
		if (top.NextComponentToTry == nextComponents.size())
		{
			executionStack.pop_back();
			continue;
		}

		const Component& nextComponent = components[top.StartingPins][top.NextComponentToTry];
		top.NextComponentToTry++;

		if (((top.CurrentBridge[0] & nextComponent.Id[0]) + (top.CurrentBridge[1] & nextComponent.Id[1])) == 0)
		{
			ExecutionContext newContext = top;
			newContext.CurrentBridge[0] |= nextComponent.Id[0];
			newContext.CurrentBridge[1] |= nextComponent.Id[1];
			newContext.CurrentStrength += nextComponent.From + nextComponent.To;
			newContext.StartingPins = nextComponent.To;
			newContext.NextComponentToTry = 0;
			executionStack.push_back(newContext);

			int32_t length = popcount(newContext.CurrentBridge[0]) + popcount(newContext.CurrentBridge[1]);
			if (length > maxLength)
			{
				maxLength = length;
				strengthOfMaxLength = newContext.CurrentStrength;
			}
			else if (length == maxLength)
			{
				strengthOfMaxLength = max(strengthOfMaxLength, newContext.CurrentStrength);
			}
		}
	}

	return strengthOfMaxLength;
}

void Puzzle24_A_2017()
{
	vector<vector<Component>> components = ParseComponents();
	int32_t answer = StrongestBridge(components);

	PuzzleOutput::Submit(2017, 24, 1, answer);
}

void Puzzle24_B_2017()
{
	vector<vector<Component>> components = ParseComponents();
	int32_t answer = LongestBridge(components);

	PuzzleOutput::Submit(2017, 24, 2, answer);
}

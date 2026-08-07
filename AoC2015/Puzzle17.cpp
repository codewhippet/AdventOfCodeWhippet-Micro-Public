#include "stdafx.h"

using namespace std;

namespace Puzzle17_2015_Types
{
	enum class EvalState : uint32_t
	{
		EvaluateWithAndWithout,
		EvaluateWithout,
		EvaluateNone,
	};

	struct Eval
	{
		EvalState State;
		size_t ContainerIndex;
		int32_t TargetLitres;
		int32_t ContainersUsed;
	};
}

using namespace Puzzle17_2015_Types;

static int32_t StorageCombinations(const vector<int32_t>& containers, const int32_t targetLitres, vector<Eval>* evalStack, vector<int32_t>* solutionCount)
{
	int32_t combinations = 0;

	evalStack->push_back({ EvalState::EvaluateWithAndWithout, 0, targetLitres, 0 });
	while (!evalStack->empty())
	{
		Eval& currentState = evalStack->back();
		if (currentState.TargetLitres == 0)
		{
			(*solutionCount)[currentState.ContainersUsed]++;
			combinations++;
			evalStack->pop_back();
			continue;
		}

		if (currentState.ContainerIndex == containers.size())
		{
			evalStack->pop_back();
			continue;
		}

		switch (currentState.State)
		{
		case EvalState::EvaluateWithAndWithout:
		{
			evalStack->push_back({
				EvalState::EvaluateWithAndWithout,
				currentState.ContainerIndex + 1,
				currentState.TargetLitres - containers[currentState.ContainerIndex],
				currentState.ContainersUsed + 1
				});

			currentState.State = EvalState::EvaluateWithout;
		}
		break;
		case EvalState::EvaluateWithout:
		{
			evalStack->push_back({
				EvalState::EvaluateWithAndWithout,
				currentState.ContainerIndex + 1,
				currentState.TargetLitres,
				currentState.ContainersUsed
				});

			currentState.State = EvalState::EvaluateNone;
		}
		break;
		case EvalState::EvaluateNone:
			evalStack->pop_back();
			break;
		}
	}

	return combinations;
}

void Puzzle17_A_2015()
{
	const int64_t targetLitres = 150;

	vector<int32_t> containers;
	containers.reserve(32);
	while (PuzzleInput::NextLine())
	{
		containers.push_back(Parse::GetInt32());
	}

	vector<Eval> evalStack;
	evalStack.reserve(32);

	vector<int32_t> solutionCount(containers.size());
	int32_t answer = StorageCombinations(containers, targetLitres, &evalStack, &solutionCount);
	
	return PuzzleOutput::Submit(2015, 17, 1, answer);
}

void Puzzle17_B_2015()
{
	const int64_t targetLitres = 150;

	vector<int32_t> containers;
	containers.reserve(32);
	while (PuzzleInput::NextLine())
	{
		containers.push_back(Parse::GetInt32());
	}

	vector<Eval> evalStack;
	evalStack.reserve(32);

	vector<int32_t> solutionCount(containers.size() + 1);
	StorageCombinations(containers, targetLitres, &evalStack, &solutionCount);

	int32_t answer = *ranges::find_if_not(solutionCount, [](int32_t count) { return count == 0; });

	return PuzzleOutput::Submit(2015, 17, 2, answer);
}

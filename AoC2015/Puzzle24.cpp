#include "stdafx.h"
#include <bit>

using namespace std;

namespace Puzzle24_2015_Types
{
	struct Group
	{
		int32_t GroupSize;
		int64_t QuantumEntanglement;
	};

	enum class EvalStage
	{
		CheckForAnswer,
		TryWith,
		TryWithout,
		End,
	};

	struct EvalState
	{
		EvalStage Stage;
		size_t WeightIndex = 0;
		uint32_t Combination = 0;
		int32_t GroupSize = 0;
		int32_t Weight = 0;
		int64_t Entanglement = 1;
	};
}

using namespace Puzzle24_2015_Types;

static int64_t FindSmallestQuantumEntanglement(const vector<int32_t>& weights, int32_t targetWeight)
{
	vector<EvalState> evalStack;
	evalStack.reserve(32);
	evalStack.push_back({ EvalStage::CheckForAnswer });

	Group minEntanglement{ numeric_limits<int32_t>::max(), numeric_limits<int64_t>::max() };
	while (!evalStack.empty())
	{
		EvalState& current = evalStack.back();
		switch (current.Stage)
		{
		case EvalStage::CheckForAnswer:
			{
				if ((current.Weight > targetWeight) || (current.GroupSize > minEntanglement.GroupSize))
				{
					current.Stage = EvalStage::End;
				}
				else if (current.Weight == targetWeight)
				{
					if (current.GroupSize < minEntanglement.GroupSize)
					{
						// It's necessary to 'reset' the QE here because some of the bigger groups overflow into negatives
						minEntanglement.GroupSize = current.GroupSize;
						minEntanglement.QuantumEntanglement = current.Entanglement;
					}
					else if (current.GroupSize == minEntanglement.GroupSize)
					{
						minEntanglement.QuantumEntanglement = min(minEntanglement.QuantumEntanglement, current.Entanglement);
					}

					current.Stage = EvalStage::End;
				}
				else
				{
					current.Stage = (current.WeightIndex == weights.size() ? EvalStage::End : EvalStage::TryWith);
				}
			}
			break;

		case EvalStage::TryWith:
			{
				current.Stage = EvalStage::TryWithout;

				EvalState newState = current;
				newState.Stage = EvalStage::CheckForAnswer;
				newState.WeightIndex += 1;
				newState.Combination |= 1 << current.WeightIndex;
				newState.GroupSize += 1;
				newState.Weight += weights[current.WeightIndex];
				newState.Entanglement *= weights[current.WeightIndex];
				evalStack.push_back(newState);
			}
			break;

		case EvalStage::TryWithout:
			{
				current.Stage = EvalStage::End;

				EvalState newState = current;
				newState.Stage = EvalStage::CheckForAnswer;
				newState.WeightIndex += 1;
				evalStack.push_back(newState);
			}
			break;

		case EvalStage::End:
			evalStack.pop_back();
			break;
		}
	}

	return minEntanglement.QuantumEntanglement;
}

void Puzzle24_A_2015()
{
	vector<int32_t> weights;
	weights.reserve(32);
	while (PuzzleInput::NextLine())
	{
		weights.push_back(Parse::GetInt32());
	}
	ranges::reverse(weights);

	int32_t totalWeight = accumulate(weights.begin(), weights.end(), 0);
	int64_t answer = FindSmallestQuantumEntanglement(weights, totalWeight / 3);

	return PuzzleOutput::Submit(2015, 24, 1, answer);
}

void Puzzle24_B_2015()
{
	vector<int32_t> weights;
	weights.reserve(32);
	while (PuzzleInput::NextLine())
	{
		weights.push_back(Parse::GetInt32());
	}
	ranges::reverse(weights);

	int32_t totalWeight = accumulate(weights.begin(), weights.end(), 0);
	int64_t answer = FindSmallestQuantumEntanglement(weights, totalWeight / 4);

	return PuzzleOutput::Submit(2015, 24, 2, answer);
}

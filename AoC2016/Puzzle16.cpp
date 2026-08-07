#include "stdafx.h"

using namespace std;

namespace Puzzle16_2016_Types
{
	class DragonGeneratorBits
	{
	public:
		DragonGeneratorBits(const vector<bool>& input)
			: OriginalInputSize(input.size())
			, InputCounter(OriginalInputSize)
		{
			for (size_t i = 0; i < input.size(); i++)
			{
				ForwardAndFlippedInput |= uint64_t(input[i]) << i;
				ForwardAndFlippedInput |= uint64_t(!input[i]) << (((OriginalInputSize * 2) - 1) - i);
			}
		}

		size_t Next()
		{
			size_t next;
			if (InputCounter == 0)
			{
				next = DragonCurve(DragonCounter + 1);
				DragonCounter++;
				InputCounter = OriginalInputSize;
			}
			else
			{
				next = ForwardAndFlippedInput & 0x1;
				ForwardAndFlippedInput = (ForwardAndFlippedInput >> 1) | (ForwardAndFlippedInput << ((OriginalInputSize * 2) - 1));
				InputCounter--;
			}

			return next;
		}

	private:

		size_t DragonCurve(size_t index)
		{
			while ((index & 1) == 0)
				index >>= 1;

			return (index & 0x3) == 3;
		}

		uint64_t ForwardAndFlippedInput = 0;
		size_t OriginalInputSize = 0;

		size_t InputCounter = 0;

		size_t DragonCounter = 0;
	};

	class ChecksumAccumulatorBits
	{
	public:
		ChecksumAccumulatorBits(size_t stages)
		{
			OutputBit = size_t(1) << (stages - 1);
		}

		bool HasOutput() const
		{
			return (AccumulatorHasValue & OutputBit);
		}

		bool GetOutput()
		{
			assert(HasOutput());
			AccumulatorHasValue &= ~OutputBit;
			return (Accumulators & OutputBit);
		}

		void AddValue(size_t next)
		{
			assert(next < 2);
			assert(HasOutput() == false);
			AccumulatorHasValue++;

			const size_t propagateCount = countr_zero(AccumulatorHasValue);
			size_t nextBit = 1;
			size_t nextValue = next;
			for (size_t i = 0; i < propagateCount; i++)
			{
				nextValue = (~(Accumulators ^ nextValue) & nextBit) << 1;
				nextBit <<= 1;
			}
			Accumulators = (Accumulators & ~nextBit) | (nextValue);
		}

	private:
		size_t Accumulators = 0;
		size_t AccumulatorHasValue = 0;
		size_t OutputBit;
	};
}

using namespace Puzzle16_2016_Types;

static vector<bool> ParseStartingState()
{
	vector<bool> startingState;
	startingState.reserve(17);
	for (int c = PuzzleInput::GetChar(); c != EOF; c = PuzzleInput::GetChar())
	{
		switch (c)
		{
		case '0':
			startingState.push_back(false);
			break;
		case '1':
			startingState.push_back(true);
			break;
		}
	}
	return startingState;
}

static size_t CalculateNumTransforms(const size_t minimumLength, const size_t inputLength)
{
	size_t numTransforms = 0;

	size_t transformedLength = inputLength;
	while (transformedLength < minimumLength)
	{
		transformedLength = (transformedLength * 2) + 1;
		numTransforms++;
	}

	return numTransforms;
}

void Puzzle16_A_2016()
{
	const size_t minimumLength = 272;

	vector<bool> state = ParseStartingState();

	const size_t numTransforms = CalculateNumTransforms(minimumLength, state.size());

	DragonGeneratorBits generator(state);
	ChecksumAccumulatorBits acc(numTransforms + 1);

	string answer;
	answer.reserve(state.size());
	for (size_t i = 0; i < minimumLength; i++)
	{
		acc.AddValue(generator.Next());
		if (acc.HasOutput())
		{
			answer += acc.GetOutput() ? '1' : '0';
		}
	}

	return PuzzleOutput::Submit(2016, 16, 1, answer.c_str());
}

void Puzzle16_B_2016()
{
	const size_t minimumLength = 35651584;

	vector<bool> state = ParseStartingState();

	const size_t numTransforms = CalculateNumTransforms(minimumLength, state.size());

	DragonGeneratorBits generator(state);
	ChecksumAccumulatorBits acc(numTransforms + 1);

	string answer;
	answer.reserve(state.size());
	for (size_t i = 0; i < minimumLength; i++)
	{
		acc.AddValue(generator.Next());
		if (acc.HasOutput())
		{
			answer += acc.GetOutput() ? '1' : '0';
		}
	}

	return PuzzleOutput::Submit(2016, 16, 2, answer.c_str());
}

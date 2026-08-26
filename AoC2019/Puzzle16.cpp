#include "stdafx.h"

using namespace std;

namespace Puzzle16_2019_Types
{
	struct ReversedRepeatedSignal
	{
		const vector<int32_t>& Signal;
		int32_t Index = 0;

		int Next()
		{
			int32_t signalSize = static_cast<int32_t>(Signal.size());
			Index = (Index + signalSize - 1) % signalSize;
			return Signal[Index];
		}
	};
}

using namespace Puzzle16_2019_Types;

static void StepSignal(vector<int32_t>* signal)
{
	array<int32_t, 4> coefficientPattern{ 1, 0, -1, 0 };
	for (size_t i = 0; i < signal->size(); i++)
	{
		int32_t newSignalValue = 0;

		int32_t coefficientState = 0;
		int32_t stateCounter = static_cast<int32_t>(i + 1);
		for (size_t j = i; j < signal->size(); j++)
		{
			int32_t coefficient = coefficientPattern[coefficientState];
			newSignalValue += (*signal)[j] * coefficient;
			if (--stateCounter == 0)
			{
				coefficientState = (coefficientState + 1) & 0x3;
				stateCounter = static_cast<int32_t>(i + 1);
			}
		}

		(*signal)[i] = abs(newSignalValue) % 10;
	}
}

void Puzzle16_A_2019()
{
	vector<int32_t> signal;
	signal.reserve(650);
	for (int c = PuzzleInput::GetChar(); c != '\n'; c = PuzzleInput::GetChar())
	{
		signal.push_back(c - '0');
	}

	for (size_t i = 0; i < 100; i++)
	{
		StepSignal(&signal);
	}

	auto answerDigits = signal | views::take(8);
	int32_t answer = accumulate(answerDigits.begin(), answerDigits.end(), 0,
		[](int32_t a, int32_t b)
		{
			return a * 10 + b;
		});

	PuzzleOutput::Submit(2019, 16, 1, answer);
}

void Puzzle16_B_2019()
{
	vector<int32_t> signal;
	signal.reserve(650);
	for (int c = PuzzleInput::GetChar(); c != '\n'; c = PuzzleInput::GetChar())
	{
		signal.push_back(c - '0');
	}

	auto offsetDigits = signal | views::take(7);
	int32_t offset = accumulate(offsetDigits.begin(), offsetDigits.end(), 0,
		[](int32_t a, int32_t b)
		{
			return a * 10 + b;
		});

	int32_t signalToProcess = static_cast<int32_t>(signal.size()) * 10000 - offset;
	ReversedRepeatedSignal baseSignal(signal);

	array<int32_t, 8> digits = {};

	vector<int32_t> phaseHistory(100);
	for (int32_t i = 0; i < signalToProcess; i++)
	{
		int32_t s = baseSignal.Next();
		for (int32_t phase = 0; phase < static_cast<int32_t>(phaseHistory.size()); phase++)
		{
			s += phaseHistory[phase];
			phaseHistory[phase] = s;
		}

		// Periodically reduce the phase history to stop it exploding
		if ((i & 0x3) == 0)
		{
			for (int32_t phase = 0; phase < static_cast<int32_t>(phaseHistory.size()); phase++)
			{
				phaseHistory[phase] = phaseHistory[phase] % 10;
			}
		}

		digits[i & (digits.size() - 1)] = s % 10;
	}

	int32_t digitsIndex = signalToProcess;

	int32_t answer = 0;
	for (int32_t i = 0; i < 8; i++)
	{
		digitsIndex += static_cast<int32_t>(digits.size()) - 1;
		answer = answer * 10 + digits[digitsIndex & (digits.size() - 1)];
	}

	PuzzleOutput::Submit(2019, 16, 2, answer);
}

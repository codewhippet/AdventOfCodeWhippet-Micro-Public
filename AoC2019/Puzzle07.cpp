#include "stdafx.h"
#include "Intputer.h"

using namespace std;

namespace Puzzle07_2019_Types
{
	struct Polynomial
	{
		int32_t A;
		int32_t B;

		int32_t operator()(int32_t x) const
		{
			return (A * x) + B;
		}
	};

	struct BlackBox
	{
		array<Polynomial, 10> Func;
	};
}

using namespace Puzzle07_2019_Types;

static Polynomial ExtractPolynomial(const vector<int32_t>& program, int32_t phase)
{
	Polynomial p;

	{
		uIntputerWithIO<int32_t> puter;
		puter.CopyProgram(program);
		puter.GetReadQueue()->push_back(phase);
		puter.GetReadQueue()->push_back(0);

		auto exec = puter.Execute();
		assert(exec == uIntputer<int32_t>::ExecutionResult::Finished);
		(void)exec;

		p.B = puter.GetWriteQueue()->back();
	}

	{
		uIntputerWithIO<int32_t> puter;
		puter.CopyProgram(program);
		puter.GetReadQueue()->push_back(phase);
		puter.GetReadQueue()->push_back(1);

		auto exec = puter.Execute();
		assert(exec == uIntputer<int32_t>::ExecutionResult::Finished);
		(void)exec;

		p.A = puter.GetWriteQueue()->back() - p.B;
	}

	return p;
}

static BlackBox ExtractBlackBox(const vector<int32_t>& program, int32_t phase)
{
	// Black boxes are a combination of 10 of these operations
	const array<Polynomial, 3> operations =
	{
		Polynomial{ 2, 0 }, // * 2
		Polynomial{ 1, 1 }, // + 1
		Polynomial{ 1, 2 }, // + 2
	};

	uIntputerWithIO<int32_t> puter;
	puter.CopyProgram(program);
	puter.GetReadQueue()->push_back(phase);

	BlackBox bb;
	for (size_t i = 0; i < 10; i++)
	{
		puter.GetReadQueue()->push_back(0);
		puter.Execute();

		bb.Func[i] = operations[puter.GetWriteQueue()->back()];
	}
	return bb;
}

static int32_t SignalFromPolynomials(const vector<Polynomial>& polynomials, const vector<int32_t>& phases)
{
	int32_t signal = 0;
	for (int32_t phase : phases)
	{
		signal = polynomials[phase](signal);
	}
	return signal;
}

static int32_t SignalFromBlackBoxes(const vector<BlackBox>& blackBoxes, const vector<int32_t>& phases)
{
	int32_t signal = 0;
	for (size_t stage = 0; stage < 10; stage++)
	{
		for (int32_t phase : phases)
		{
			signal = blackBoxes[phase - 5].Func[stage](signal);
		}
	}
	return signal;
}

void Puzzle07_A_2019()
{
	vector<int32_t> startingProgram;
	startingProgram.reserve(600);
	while (PuzzleInput::NextLine())
	{
		startingProgram.push_back(Parse::GetInt32());
	}

	vector<Polynomial> amplifiers;
	amplifiers.reserve(5);

	vector<int32_t> phases{ 0, 1, 2, 3, 4 };
	for (int32_t phase : phases)
	{
		amplifiers.push_back(ExtractPolynomial(startingProgram, phase));
	}

	MaxValue<int32_t> maximumSignal;
	do
	{
		maximumSignal.Update(SignalFromPolynomials(amplifiers, phases));

	} while (next_permutation(phases.begin(), phases.end()));

	int32_t answer = maximumSignal.Get();
	PuzzleOutput::Submit(2019, 7, 1, answer);
}

void Puzzle07_B_2019()
{
	vector<int32_t> startingProgram;
	startingProgram.reserve(600);
	while (PuzzleInput::NextLine())
	{
		startingProgram.push_back(Parse::GetInt32());
	}

	vector<BlackBox> amplifiers;
	amplifiers.reserve(5);

	vector<int32_t> phases{ 5, 6, 7, 8, 9 };
	for (int32_t phase : phases)
	{
		amplifiers.push_back(ExtractBlackBox(startingProgram, phase));
	}

	MaxValue<int32_t> maximumSignal;
	do
	{
		maximumSignal.Update(SignalFromBlackBoxes(amplifiers, phases));

	} while (next_permutation(phases.begin(), phases.end()));

	int32_t answer = maximumSignal.Get();
	PuzzleOutput::Submit(2019, 7, 2, answer);
}

#include "stdafx.h"
#include "Intputer.h"
#include <iostream>

using namespace std;

namespace Puzzle21_2019_Types
{
}

using namespace Puzzle21_2019_Types;

#if 0
static void InteractiveMode(Intputer* puter)
{
	for (int lineIndex = 0; lineIndex < 20; lineIndex++)
	{
		puter->GetWriteQueue()->clear();

		auto exec = puter->Execute();
		assert(exec != Intputer::ExecutionResult::Exception);

		ranges::for_each(*puter->GetWriteQueue() | views::filter([](int64_t c) { return c < 256; }),
			[](int64_t c) { putc((int)c, stdout); });

		printf("[% 2d] $: ", lineIndex);
		string line;
		getline(cin, line);

		if (exec == Intputer::ExecutionResult::Finished)
			break;

		ranges::copy(line, back_inserter(*puter->GetReadQueue()));
		puter->GetReadQueue()->push_back('\n');
	}
}
#endif

void Puzzle21_A_2019()
{
	uIntputer<int32_t> puter(2200);

	uIntputerIO<int32_t> io;
	puter.SetReadWriteQueues(&io);

	string springbotProgram =
		R"(OR A T
		AND B T
		AND C T
		NOT T J
		AND D J
		WALK
		)";

	ranges::copy(springbotProgram | views::filter([](char c) { return c != '\t'; }), back_inserter(io.Read));

	auto exec = puter.Execute();
	assert(exec == uIntputerExecutionResult::Finished);
	(void)exec;

	int32_t answer = io.Write.back();

	PuzzleOutput::Submit(2019, 21, 1, answer);
}

void Puzzle21_B_2019()
{
	uIntputer<int32_t> puter(2200);

	uIntputerIO<int32_t> io;
	puter.SetReadWriteQueues(&io);

	string springbotProgram =
		R"(OR A T
		AND B T
		AND C T
		NOT T J
		AND D J
		NOT E T
		NOT T T
		OR H T
		AND T J
		RUN
		)";

	ranges::copy(springbotProgram | views::filter([](char c) { return c != '\t'; }), back_inserter(io.Read));

	puter.Execute();

	int32_t answer = io.Write.back();

	PuzzleOutput::Submit(2019, 21, 2, answer);
}

#include "stdafx.h"
#include "Intputer.h"

using namespace std;

namespace Puzzle05_2019_Types
{
}

using namespace Puzzle05_2019_Types;

void Puzzle05_A_2019()
{
	uIntputer<int32_t> puter(1024);

	deque<int32_t> in{ 1 };
	deque<int32_t> out;
	puter.SetReadWriteQueues(&in, &out);

	auto exec = puter.Execute();
	assert(exec == uIntputer<int32_t>::ExecutionResult::Finished);
	(void)exec;

	int32_t answer = out.back();

	PuzzleOutput::Submit(2019, 5, 1, answer);
}

void Puzzle05_B_2019()
{
	uIntputer<int32_t> puter(1024);

	deque<int32_t> in{ 5 };
	deque<int32_t> out;
	puter.SetReadWriteQueues(&in, &out);

	auto exec = puter.Execute();
	assert(exec == uIntputer<int32_t>::ExecutionResult::Finished);
	(void)exec;

	int32_t answer = out.back();

	PuzzleOutput::Submit(2019, 5, 2, answer);
}

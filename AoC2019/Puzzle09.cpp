#include "stdafx.h"
#include "Intputer.h"

using namespace std;

namespace Puzzle09_2019_Types
{
}

using namespace Puzzle09_2019_Types;

void Puzzle09_A_2019()
{
	uIntputer<int64_t> puter;
	puter.ReadProgramFromInput(1024 + 512);

	deque<int64_t> in{ 1 };
	deque<int64_t> out;
	puter.SetReadWriteQueues(&in, &out);

	auto result = puter.Execute();
	assert(result == uIntputer<int64_t>::ExecutionResult::Finished);
	(void)result;

	int64_t answer = out.front();
	PuzzleOutput::Submit(2019, 9, 1, answer);
}

void Puzzle09_B_2019()
{
	uIntputer<int64_t> puter;
	puter.ReadProgramFromInput(1024 + 512);

	deque<int64_t> in{ 2 };
	deque<int64_t> out;
	puter.SetReadWriteQueues(&in, &out);

	auto result = puter.Execute();
	assert(result == uIntputer<int64_t>::ExecutionResult::Finished);
	(void)result;

	int64_t answer = out.front();
	PuzzleOutput::Submit(2019, 9, 2, answer);
}

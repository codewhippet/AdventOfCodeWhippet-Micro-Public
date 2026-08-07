#include "stdafx.h"
#include "Intputer.h"

using namespace std;

static string_view dummy =
R"()";

namespace Puzzle05_2019_Types
{
}

using namespace Puzzle05_2019_Types;

static void Puzzle05_A(const string &filename)
{
	(void)filename;
	ifstream input(filename);
	//istringstream input(dummy);

	Intputer puter(input);

	deque<int64_t> in{ 1 };
	deque<int64_t> out;
	puter.SetReadWriteQueues(&in, &out);
	
	auto exec = puter.Execute();
	assert(exec == Intputer::ExecutionResult::Finished);
	(void)exec;

	int64_t answer = out.back();

	printf("[2019] Puzzle05_A: %" PRId64 "\n", answer);
}


static void Puzzle05_B(const string& filename)
{
	(void)filename;
	ifstream input(filename);
	//istringstream input(dummy);

	Intputer puter(input);

	deque<int64_t> in{ 5 };
	deque<int64_t> out;
	puter.SetReadWriteQueues(&in, &out);

	auto exec = puter.Execute();
	assert(exec == Intputer::ExecutionResult::Finished);
	(void)exec;

	int64_t answer = out.back();

	printf("[2019] Puzzle05_B: %" PRId64 "\n", answer);
}

void Puzzle05_A_2019()
{
	Puzzle05_A(R"(z:\AoCInput\2019\Puzzle05.txt)");
}

void Puzzle05_B_2019()
{
	Puzzle05_B(R"(z:\AoCInput\2019\Puzzle05.txt)");
}

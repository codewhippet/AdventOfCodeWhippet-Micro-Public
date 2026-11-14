#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle01_2026_Types
{
}

using namespace Puzzle01_2026_Types;

static void Puzzle01_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle01_A: %" PRId64 "\n", answer);
}

static void Puzzle01_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle01_B: %" PRId64 "\n", answer);
}

void Puzzle01_A_2026()
{
	Puzzle01_A(R"(z:\AoCInput\2026\Puzzle01.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 1, 1, answer);
}

void Puzzle01_B_2026()
{
	Puzzle01_B(R"(z:\AoCInput\2026\Puzzle01.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 1, 2, answer);
}

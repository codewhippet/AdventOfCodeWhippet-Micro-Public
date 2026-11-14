#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle02_2026_Types
{
}

using namespace Puzzle02_2026_Types;

static void Puzzle02_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle02_A: %" PRId64 "\n", answer);
}

static void Puzzle02_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle02_B: %" PRId64 "\n", answer);
}

void Puzzle02_A_2026()
{
	Puzzle02_A(R"(z:\AoCInput\2026\Puzzle02.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 2, 1, answer);
}

void Puzzle02_B_2026()
{
	Puzzle02_B(R"(z:\AoCInput\2026\Puzzle02.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 2, 2, answer);
}

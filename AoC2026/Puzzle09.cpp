#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle09_2026_Types
{
}

using namespace Puzzle09_2026_Types;

static void Puzzle09_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle09_A: %" PRId64 "\n", answer);
}

static void Puzzle09_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle09_B: %" PRId64 "\n", answer);
}

void Puzzle09_A_2026()
{
	Puzzle09_A(R"(z:\AoCInput\2026\Puzzle09.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 9, 1, answer);
}

void Puzzle09_B_2026()
{
	Puzzle09_B(R"(z:\AoCInput\2026\Puzzle09.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 9, 2, answer);
}

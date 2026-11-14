#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle07_2026_Types
{
}

using namespace Puzzle07_2026_Types;

static void Puzzle07_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle07_A: %" PRId64 "\n", answer);
}

static void Puzzle07_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle07_B: %" PRId64 "\n", answer);
}

void Puzzle07_A_2026()
{
	Puzzle07_A(R"(z:\AoCInput\2026\Puzzle07.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 7, 1, answer);
}

void Puzzle07_B_2026()
{
	Puzzle07_B(R"(z:\AoCInput\2026\Puzzle07.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 7, 2, answer);
}

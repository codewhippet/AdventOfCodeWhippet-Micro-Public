#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle10_2026_Types
{
}

using namespace Puzzle10_2026_Types;

static void Puzzle10_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle10_A: %" PRId64 "\n", answer);
}

static void Puzzle10_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle10_B: %" PRId64 "\n", answer);
}

void Puzzle10_A_2026()
{
	Puzzle10_A(R"(z:\AoCInput\2026\Puzzle10.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 10, 1, answer);
}

void Puzzle10_B_2026()
{
	Puzzle10_B(R"(z:\AoCInput\2026\Puzzle10.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 10, 2, answer);
}

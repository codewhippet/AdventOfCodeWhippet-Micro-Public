#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle08_2026_Types
{
}

using namespace Puzzle08_2026_Types;

static void Puzzle08_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle08_A: %" PRId64 "\n", answer);
}

static void Puzzle08_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle08_B: %" PRId64 "\n", answer);
}

void Puzzle08_A_2026()
{
	Puzzle08_A(R"(z:\AoCInput\2026\Puzzle08.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 8, 1, answer);
}

void Puzzle08_B_2026()
{
	Puzzle08_B(R"(z:\AoCInput\2026\Puzzle08.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 8, 2, answer);
}

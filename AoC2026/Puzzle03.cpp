#include "stdafx.h"

using namespace std;

static string dummy =
R"()";

namespace Puzzle03_2026_Types
{
}

using namespace Puzzle03_2026_Types;

static void Puzzle03_A(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle03_A: %" PRId64 "\n", answer);
}

static void Puzzle03_B(const string& filename)
{
	(void)filename;
	//ifstream input(filename);
	istringstream input(dummy);

	int64_t answer = 0;
	printf("[2026] Puzzle03_B: %" PRId64 "\n", answer);
}

void Puzzle03_A_2026()
{
	Puzzle03_A(R"(z:\AoCInput\2026\Puzzle03.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 3, 1, answer);
}

void Puzzle03_B_2026()
{
	Puzzle03_B(R"(z:\AoCInput\2026\Puzzle03.txt)");

	int32_t answer = 0;
	PuzzleOutput::Submit(2026, 3, 2, answer);
}
